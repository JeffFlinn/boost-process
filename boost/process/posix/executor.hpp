
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_EXECUTOR_HPP
#define BOOST_PROCESS_POSIX_EXECUTOR_HPP

#include <boost/process/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/optional.hpp>
#include <boost/process/posix/child.hpp>
#include <boost/process/posix/initializers/initializer.hpp>

#include <set>
#include <vector>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

//////////////////////////////////////////////////////////////////
#define BOOST_PROCESS_EXECUTOR_METHOD(fnc)                        \
                                                                  \
template<typename INITIALIZERS> void fnc(const INITIALIZERS& izs) \
{                                                                 \
    boost::fusion::for_each(izs, initializer::lazy::fnc<executor>(*this));  \
}
//////////////////////////////////////////////////////////////////

namespace boost { namespace process { namespace posix {

    struct executor 
    {
        typedef boost::filesystem::path      path;
        typedef char                    char_type;
        typedef int          file_descriptor_type;
        typedef pid_t                    pid_type;

        executor() : m_exe(), m_arg_ptrs(), m_env_vars_ptrs(0), m_fd_map(), m_pid(-1), m_optional_error_pipe() {}
        
        template<typename INITIALIZERS> child exec(const INITIALIZERS& izs)
        {
            pre_fork_parent(izs);
            
            if(m_optional_error_pipe) (*m_optional_error_pipe).pre_fork_parent(*this);
            
            switch(m_pid = fork())
            {
                case  0:   post_fork_child (izs); break; 
                default:   post_fork_parent(izs); break; 
                case -1: failed_fork_parent(izs); break;
            }
            
            return child(m_pid);
        }

        template<typename INITIALIZERS> void post_fork_child(const INITIALIZERS& izs)
        {
            boost::fusion::for_each(izs, initializer::lazy::post_fork_child<executor>(*this));
            
            m_fd_map.set_child_std_fds();
            m_fd_map. close_unused_fds();
        
            execve(m_exe.c_str(), m_arg_ptrs.ptr(), m_env_vars_ptrs); // doesn't return on success
            
            failed_exec_child(izs);
        }
        
        BOOST_PROCESS_EXECUTOR_METHOD(   pre_fork_parent)
        BOOST_PROCESS_EXECUTOR_METHOD(  post_fork_parent)
        BOOST_PROCESS_EXECUTOR_METHOD(failed_fork_parent)
        BOOST_PROCESS_EXECUTOR_METHOD(failed_exec_child )

        void set_std_in (file_descriptor_type fd) { m_fd_map.set_std_in (fd); }
        void set_std_out(file_descriptor_type fd) { m_fd_map.set_std_out(fd); }
        void set_std_err(file_descriptor_type fd) { m_fd_map.set_std_err(fd); }
        void set_in_use (file_descriptor_type fd) { m_fd_map.set_in_use (fd); }
        
        void use_error_pipe() 
        { 
            if(!m_optional_error_pipe) 
            {
                m_optional_error_pipe = error_directed_pipe();
                
                (*m_optional_error_pipe).post_fork_child(*this);
            }
        }
        
        void child_throw(int err)
        {
            if(m_optional_error_pipe) (*m_optional_error_pipe).child_throw(err);    
        }

        template<class EXC> void parent_throw_if() { if(m_optional_error_pipe) (*m_optional_error_pipe).parent_throw_if<EXC>(); }
        
        struct error_directed_pipe 
        {
            typedef int file_descriptor_type;
            
            mutable file_descriptor_type  m_read_end;
            mutable file_descriptor_type m_write_end;
            mutable int                      m_errno;
            
             error_directed_pipe() : m_read_end(-1), m_write_end(-1), m_errno(0) {}
            ~error_directed_pipe()
            {
                if( m_read_end!=-1) close( m_read_end);
                if(m_write_end!=-1) close(m_write_end);
            }
            
            void pre_fork_parent(executor& e) const
            {
                {
                    int pipe_fds[2] = {-1, -1};
                    
                    ::pipe(pipe_fds);
                    
                     m_read_end = ensure_user_fd_index(pipe_fds[0]); if( m_read_end != pipe_fds[0]) close(pipe_fds[0]);
                    m_write_end = ensure_user_fd_index(pipe_fds[1]); if(m_write_end != pipe_fds[1]) close(pipe_fds[1]);
                }
                
                signal(SIGPIPE, SIG_IGN); // Who's to say this doesn't change elsewhere
                
                fcntl( m_read_end, F_SETFD, FD_CLOEXEC|fcntl( m_read_end, F_GETFD, 0)); 
                fcntl(m_write_end, F_SETFD, FD_CLOEXEC|fcntl(m_write_end, F_GETFD, 0));
                
                e.set_in_use(m_write_end); 
            }
            
            void post_fork_child(executor&) const
            {
                close(m_read_end); m_read_end = -1;
            }
            
            void child_throw(int err)
            {
                m_errno = err;
                
                write(m_write_end, &m_errno, sizeof(m_errno));
                
                close(m_write_end);
                
                _exit(127);    
            }
            
            template<class EXC> void parent_throw_if()
            {
                if(-1 != m_write_end) { close(m_write_end); m_write_end = -1; }
                
                if(read(m_read_end, &m_errno, sizeof(m_errno)))
                {
                    close(m_read_end);
                    
                    if(m_errno) throw EXC(boost::system::error_code(m_errno, boost::system::get_system_category()));
                }
                close(m_read_end);
            }
            
            static file_descriptor_type ensure_user_fd_index(file_descriptor_type fd)
            {
                return executor::parent_file_descriptor_map::ensure_user_fd_index(fd);;
            }
        };
        
        struct arg_ptrs
        {
            arg_ptrs() : m_arg_ptrs(2, (char*)0) {} // front is for app name, back is trailing null 
        
            void assign_app(char_type* n) { m_arg_ptrs.front() = n; }
            void  push_back(char_type* a) { m_arg_ptrs. back() = a; m_arg_ptrs.push_back(0); }
            
            char_type* const* ptr() const { return &m_arg_ptrs[0]; }
        
        private:
            typedef std::vector<char_type*> arg_ptrs_vec;
            
            arg_ptrs_vec m_arg_ptrs;
        };
       
        struct parent_file_descriptor_map
        {
            typedef std::set<file_descriptor_type> in_use_set_type;
        
            parent_file_descriptor_map()
            : m_dupd_parent_std_in (-1)
            , m_dupd_parent_std_out(-1)
            , m_dupd_parent_std_err(-1)
            {}
            
            // call from child process
            void set_std_in (file_descriptor_type fd) { m_dupd_parent_std_in  = ensure_user_fd_index(fd); } 
            void set_std_out(file_descriptor_type fd) { m_dupd_parent_std_out = ensure_user_fd_index(fd); }
            void set_std_err(file_descriptor_type fd) { m_dupd_parent_std_err = ensure_user_fd_index(fd); }

            void set_child_std_fds()
            {
                dup2((-1!=m_dupd_parent_std_in )? m_dupd_parent_std_in  : open("/dev/zero", O_RDONLY),  STDIN_FILENO);
                dup2((-1!=m_dupd_parent_std_out)? m_dupd_parent_std_out : open("/dev/null", O_WRONLY), STDOUT_FILENO);
                dup2((-1!=m_dupd_parent_std_err)? m_dupd_parent_std_err : open("/dev/null", O_WRONLY), STDERR_FILENO);
            }
            
            void set_in_use(file_descriptor_type fd) { m_in_use_set.insert(fd); }
            
            void close_unused_fds()
            {
                 for(file_descriptor_type fd = min_user_file_descriptor() ; fd < max_file_descriptor() ; ++fd ) { close_unused_fd(fd); }
            }
            
            void close_unused_fd(file_descriptor_type fd)
            {
                 if(!m_in_use_set.count(fd)) { close(fd); }
            }

            static file_descriptor_type ensure_user_fd_index(file_descriptor_type fd)
            {
                file_descriptor_type user_fd = fcntl(fd, F_DUPFD, min_user_file_descriptor());

                return user_fd;
            }
            
            static file_descriptor_type      max_file_descriptor() { return 1000; }
            static file_descriptor_type min_user_file_descriptor() { return 3; /*BOOST_STATIC_ASSERT((STDIN_FILENO==0) && (STDOUT_FILENO==1) && (STDERR_FILENO==2))*/ }

            file_descriptor_type m_dupd_parent_std_in ;        
            file_descriptor_type m_dupd_parent_std_out;        
            file_descriptor_type m_dupd_parent_std_err;
            in_use_set_type      m_in_use_set;
        };
        
        typedef boost::optional<error_directed_pipe> optional_error_pipe_type;

        path                       m_exe;
        arg_ptrs                   m_arg_ptrs;
        char_type**                m_env_vars_ptrs;
        parent_file_descriptor_map m_fd_map;
        pid_type                   m_pid;
        optional_error_pipe_type   m_optional_error_pipe;
        
    private:
                   executor(const executor&);
        executor& operator=(const executor&);
    };

}}}

#undef BOOST_PROCESS_EXECUTOR_METHOD

#endif
