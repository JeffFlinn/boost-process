// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_EXECUTOR_HPP
#define BOOST_PROCESS_WINDOWS_EXECUTOR_HPP

#include <boost/filesystem/path.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

#include <boost/process/windows/child.hpp>

#include <windows.h>

//////////////////////////////////////////////////////////////////
#define BOOST_PROCESS_EXECUTOR_METHOD(fnc)                        \
                                                                  \
template<typename INITIALIZERS> void fnc(const INITIALIZERS& izs) \
{                                                                 \
    boost::fusion::for_each(izs, initializer::lazy::fnc<executor>(*this));  \
}
//////////////////////////////////////////////////////////////////

namespace boost { namespace process { namespace windows {

    struct executor 
    {
        typedef boost::filesystem::path        path;
        typedef wchar_t                   char_type;
        typedef std::wstring            string_type;

        executor() 
        : m_exe()
        , m_args()
        , m_process_security_attributes_ptr(0)
        , m_thread_security_attributes_ptr(0)
        , m_inherit_handles(false)
        , m_creation_flags(0)
        , m_env_vars_ptrs(0)
        , m_working_dir()
        , m_startup_info()
        , m_process_info()
        {}
        
        ~executor()
        {
            CloseHandle(m_startup_info.hStdInput );
            CloseHandle(m_startup_info.hStdOutput);
            CloseHandle(m_startup_info.hStdError );
        }

        template<typename INITIALIZERS> child exec(const INITIALIZERS& izs)
        {
            pre_create(izs);

            switch(CreateProcessW( m_exe.wstring().c_str()
								 , m_args.ptr()
								 , m_process_security_attributes_ptr
								 , m_thread_security_attributes_ptr
								 , m_inherit_handles
								 , m_creation_flags
								 , m_env_vars_ptrs
								 , m_working_dir.wstring().c_str()
								 ,&m_startup_info
								 ,&m_process_info))
            {
                default:   post_create(izs); break; 
                case  0: failed_create(izs); break; 
            }

            if (!CloseHandle(m_process_info.hThread))
            {
                throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
            }

            return child(m_process_info.hProcess);
        }

        BOOST_PROCESS_EXECUTOR_METHOD(   pre_create)
        BOOST_PROCESS_EXECUTOR_METHOD(  post_create)
        BOOST_PROCESS_EXECUTOR_METHOD(failed_create)

        struct args
        {
            args() : m_app(), m_other_args(), m_all_args() {} 

            void assign_app(const string_type& n) { m_app = n; }
            void  push_back(const string_type& a) { (m_other_args += L' ') += a; }

            char_type* ptr() { prep_all_args(); return &m_all_args[0]; }

        private:
            typedef std::vector<char_type> arg_ptrs_vec;
            
            void prep_all_args()
            {
                m_all_args.assign   (m_app.begin(), m_app.end());
                m_all_args.push_back(L' ');
                m_all_args.insert   (m_all_args.end(), m_other_args.begin(), m_other_args.end());
                m_all_args.push_back(0);
                
                string_type allcheck(m_all_args.begin(), m_all_args.end());
            }

            string_type  m_app;
            string_type  m_other_args;
            arg_ptrs_vec m_all_args;
        };
        
        void set_std_in (const HANDLE& h) { assign_std_io(m_startup_info.hStdInput , h); }
        void set_std_out(const HANDLE& h) { assign_std_io(m_startup_info.hStdOutput, h); }
        void set_std_err(const HANDLE& h) { assign_std_io(m_startup_info.hStdError , h); }
        
        void assign_std_io(HANDLE& dest, const HANDLE& src)
        {
            m_inherit_handles       = true;
            m_startup_info.dwFlags |= STARTF_USESTDHANDLES;
            
            CloseHandle(dest);
            
            DuplicateHandle(GetCurrentProcess(), src, GetCurrentProcess(), &dest, DWORD(0), false, DUPLICATE_SAME_ACCESS);
            
            SetHandleInformation(dest, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        }

        path                  m_exe;
        args                  m_args;
        LPSECURITY_ATTRIBUTES m_process_security_attributes_ptr;
        LPSECURITY_ATTRIBUTES m_thread_security_attributes_ptr;
        bool                  m_inherit_handles;
        DWORD                 m_creation_flags;
        void*                 m_env_vars_ptrs;
        path                  m_working_dir;
        STARTUPINFOW          m_startup_info;
        PROCESS_INFORMATION   m_process_info;

    private:
                   executor(const executor&);
        executor& operator=(const executor&);
    };

}}}

#undef BOOST_PROCESS_EXECUTOR_METHOD

#endif
