
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_ENVIRONMENT_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_ENVIRONMENT_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>

#if defined(__APPLE__)
#	include <crt_externs.h> // _NSGetEnviron()
#endif

namespace boost { namespace process { namespace posix {

    struct executor;

    struct environment : public initializer 
    {
		template<class Executor> void pre_create(Executor& e) const
		{
#			if defined(__APPLE__)
			
				e.m_env_vars_ptrs = *_NSGetEnviron();
			
#			else
						
				e.m_env_vars_ptrs = environ;
			
#			endif
		}
    };

}}}

#endif
