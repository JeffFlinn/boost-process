
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_MONITOR_HPP
#define BOOST_PROCESS_WINDOWS_MONITOR_HPP

#include "boost/process/windows/child.hpp"

namespace boost { namespace process { namespace windows {

    struct monitor 
    {
        monitor(const child& c) : m_child(c) {}

        DWORD join() const
        {
            if (m_child) 
            {
                if (WaitForSingleObject(m_child.handle(), INFINITE) == WAIT_FAILED)
                {
                    throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
                }

                DWORD exit_code = 0;

                if (!GetExitCodeProcess(m_child.handle(), &exit_code))
                {
                    throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
                }

                return exit_code;
            }
            else
            {
                throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
            }
        }

        template<class EXC> void join() const
        {
            if(join()) throw EXC();
        }
         
        void terminate() const
        {
            if(m_child) 
            {
                if (!TerminateProcess(m_child.handle(), EXIT_FAILURE))
                {
                    CloseHandle(m_child.handle());
                    throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
                }
                if (!CloseHandle(m_child.handle()))
                {
                    throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
                }
            }
        }
        
        child m_child;
        
        //variant<uninitialized, joinable, variant<completed, failed> > m_state;
    };
    
    struct scoped_monitor : public monitor
    {
        scoped_monitor(const child& c) : monitor(c) {}
    };
    
    template<class EXC> inline void join(const monitor& m)
    {
        m.join<EXC>();
    }

}}}

#endif // BOOST_PROCESS_WINDOWS_MONITOR_HPP
