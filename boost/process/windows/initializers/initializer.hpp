
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_HPP

namespace boost { namespace process { namespace windows {

struct initializer 
{
    template<class Executor> void    pre_create(Executor&) const {}
    template<class Executor> void   post_create(Executor&) const {}
    template<class Executor> void failed_create(Executor&) const {}

    struct lazy
    {
        template<class Executor> struct pre_create
        {
            Executor& m_e;

            pre_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { return i.pre_create(m_e); }
        };
        template<class Executor> struct post_create
        {
            Executor& m_e;

            post_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { return i.post_create(m_e); }
        };
        template<class Executor> struct failed_create
        {
            Executor& m_e;

            failed_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { return i.failed_create(m_e); }
        };
    };
};

}}}

#endif
