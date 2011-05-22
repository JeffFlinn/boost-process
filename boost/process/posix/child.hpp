
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_CHILD_HPP
#define BOOST_PROCESS_POSIX_CHILD_HPP

namespace boost { namespace process { namespace posix {

    struct child
    {
    private:
        struct safe_bool_impl { void true_value() {} };
        typedef void (safe_bool_impl::*safe_bool)();

    public:
        child(      pid_t pid) : m_pid(    pid) {}
        child(const child&  c) : m_pid(c.m_pid) {}
        child(               ) : m_pid(     -1) {}

        bool operator==(const child& rhs) const { return m_pid == rhs.m_pid; }
        bool operator!=(const child& rhs) const { return m_pid != rhs.m_pid; }

        pid_t handle() const { return m_pid; }

        operator safe_bool() const { return ((*this) != child())? &safe_bool_impl::true_value : 0; }

        pid_t m_pid;
    };

}}}

#endif // BOOST_PROCESS_WINDOWS_CHILD_HPP