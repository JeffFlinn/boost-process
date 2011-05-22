
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_CONFIG_HPP
#define BOOST_PROCESS_CONFIG_HPP

#include <boost/config.hpp>
#include <boost/system/config.hpp>

#if defined(BOOST_POSIX_API)
#   define BOOST_PROCESS_PLATFORM posix
#elif defined(BOOST_WINDOWS_API)
#   define BOOST_PROCESS_PLATFORM windows
#   define fileno _fileno
#endif

#define BOOST_PROCESS_PLATFORM_PROMOTE_PATH(COMPONENT)             <boost/process/BOOST_PROCESS_PLATFORM/COMPONENT.hpp>
#define BOOST_PROCESS_PLATFORM_PROMOTE_INITIALIZER_PATH(COMPONENT) <boost/process/BOOST_PROCESS_PLATFORM/initializers/COMPONENT.hpp>
#define BOOST_PROCESS_PLATFORM_PROMOTE_NAMESPACE(COMPONENT)        namespace boost { namespace process { using BOOST_PROCESS_PLATFORM::COMPONENT; }}

#endif
