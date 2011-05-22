
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_PATHS_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_PATHS_HPP

#include <boost/filesystem/path.hpp>
#include <boost/process/windows/initializers/initializer.hpp>

namespace boost { namespace process { namespace windows {

    struct paths : public initializer 
    {
        typedef boost::filesystem::path path;
        
        path m_exe;
        path m_working_dir;
        
        paths(const path& exe) 
        : m_exe        (exe)
        , m_working_dir(exe.parent_path())
        {}
        
        paths(const path& exe, const path& working_dir) 
        : m_exe        (exe)
        , m_working_dir(working_dir)
        {}

        template<class Executor> void pre_create(Executor& e) const
        { 
            e.m_exe         = m_exe;
            e.m_working_dir = m_working_dir;
            e.m_args.assign_app(L'"' + m_exe.wstring() + L'"'); 
        } 
    };

}}}

#endif
