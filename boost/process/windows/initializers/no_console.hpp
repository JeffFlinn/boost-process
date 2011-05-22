
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_NO_CONSOLE_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_NO_CONSOLE_HPP

#include <boost/process/config.hpp>
#include <boost/process/windows/initializers/initializer.hpp>

namespace boost { namespace process { namespace windows {

    struct no_console : public initializer 
    {
        template<class Executor> void pre_create(Executor& e) const
        {
            e.m_startup_info.dwFlags     |= STARTF_USESHOWWINDOW;
            e.m_startup_info.wShowWindow  = SW_HIDE;
        }
    };

}}}

#endif // BOOST_PROCESS_WINDOWS_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
