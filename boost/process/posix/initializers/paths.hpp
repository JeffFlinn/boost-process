
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_PATHS_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_PATHS_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>

#include <boost/filesystem/path.hpp>

#include <vector>

namespace boost { namespace process { namespace posix {

    struct paths : public initializer 
    {
        typedef boost::filesystem::path path;
        typedef std::vector<char>       path_chars;
        
                path       m_exe;
                path       m_working_dir;
        mutable path_chars m_chars;
        
        paths(const path& exe) 
        : m_exe        (exe)
        , m_working_dir(exe.branch_path())
        , m_chars      (path_as_chars(exe.string()))
        {}
        
        paths(const path& exe, const path& working_dir) 
        : m_exe        (exe)
        , m_working_dir(working_dir)
        , m_chars      (path_as_chars(exe.string()))
        {}

        static path_chars path_as_chars(const std::string& s)
        {
            path_chars val(s.begin(), s.end()); val.push_back(0);
        
            return val;  
        }
        
        template<class Executor> void  pre_fork_parent(Executor& e) const 
        { 
            e.m_exe = m_exe;
            e.m_arg_ptrs.assign_app(&m_chars[0]); 
        } 
        template<class Executor> void post_fork_child (Executor& e) const 
        {
            if(chdir(m_working_dir.c_str()) == -1)
            {
                e.child_throw(errno);
            }
        }
    };

}}}

#endif
