
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_CHILD_HPP
#define BOOST_PROCESS_WINDOWS_CHILD_HPP

#include <windows.h>

namespace boost { namespace process { namespace windows {

    struct child
    {
    private:
        struct safe_bool_impl { void true_value() {} };
        typedef void (safe_bool_impl::*safe_bool)();
    
    public:
    
         child(const HANDLE& h) : m_h(                   h) {}
         child(               ) : m_h(INVALID_HANDLE_VALUE) {}
         child(const  child& c) : m_h(INVALID_HANDLE_VALUE) 
         {
             DuplicateHandle(GetCurrentProcess(), c.handle(), GetCurrentProcess(), &m_h, DWORD(0), false, DUPLICATE_SAME_ACCESS);
         }
        
        ~child() { CloseHandle(m_h); }
        
        bool operator==(const child& rhs) const { return m_h == rhs.m_h; }
        bool operator!=(const child& rhs) const { return m_h != rhs.m_h; }

        operator safe_bool() const { return ((*this) != child())? &safe_bool_impl::true_value : 0; }

        HANDLE handle() const { return m_h; }

        HANDLE m_h;
    };

}}}

#endif // BOOST_PROCESS_WINDOWS_CHILD_HPP