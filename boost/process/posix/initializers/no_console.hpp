
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_NO_CONSOLE_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_NO_CONSOLE_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>

namespace boost { namespace process { namespace posix {

    struct no_console : public initializer {};

}}}

#endif
