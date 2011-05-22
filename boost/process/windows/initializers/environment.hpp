
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_ENVIRONMENT_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_ENVIRONMENT_HPP

#include <boost/process/config.hpp>
#include <boost/process/windows/initializers/initializer.hpp>

namespace boost { namespace process { namespace windows {

    struct environment : public initializer 
    {
		environment() : m_env(GetEnvironmentStrings())
		{

		}
		~environment()
		{
			FreeEnvironmentStringsW(LPWCH(m_env));
		}

        template<class Executor> void pre_create(Executor& e) const
        {
			e.m_creation_flags |= CREATE_UNICODE_ENVIRONMENT;
			e.m_env_vars_ptrs = m_env;
		}

	private:
		void* m_env;
	};

}}}

#endif
