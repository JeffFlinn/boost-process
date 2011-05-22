
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP

#include <boost/process/config.hpp>
#include <boost/process/windows/initializers/initializer.hpp>
#include <boost/system/system_error.hpp>

namespace boost { namespace process { namespace windows {

    template<typename X = boost::system::system_error> struct throw_on_launch_error : public initializer 
    {
        typedef X exception_type;

        template<class Executor> void failed_create(Executor&) const 
        {
            using namespace boost::system;
         
            throw exception_type(error_code(::GetLastError(), system_category())); 
        } 
    };

}}}

#endif // BOOST_PROCESS_WINDOWS_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP