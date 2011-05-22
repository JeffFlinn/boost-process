
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>
#include <boost/system/system_error.hpp>

#include <fcntl.h>

namespace boost { namespace process { namespace posix {

    struct executor;

    template<typename X = boost::system::system_error> struct throw_on_launch_error : public initializer 
    {
        typedef X exception_type;
        
        throw_on_launch_error() {}
        
        template<class Executor> void pre_fork_parent(Executor& e) const
        {
           e.use_error_pipe(); 
        }
        template<class Executor> void post_fork_parent(Executor& e) const
        {
            e.template parent_throw_if<exception_type>();
        }
        template<class Executor> void failed_exec_child(Executor& e) const
        {
            e.child_throw(errno);
        }
    
    };

}}}

#endif // BOOST_PROCESS_POSIX_INITIALIZER_THROW_ON_LAUNCH_ERROR_HPP
