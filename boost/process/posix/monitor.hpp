// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_MONITOR_HPP
#define BOOST_PROCESS_POSIX_MONITOR_HPP

#include "boost/process/posix/child.hpp"

#include <signal.h>
#include <sys/wait.h>

namespace boost { namespace process { namespace posix {

    struct monitor 
    {
        monitor(const child& c) : m_id(c.handle()) {}

        int join() const
        {
            int     status = 0;
            pid_t wait_val = 0;

            do
            {
                wait_val = waitpid(m_id, &status, 0);
            } 
            while(wait_val == -1 && errno == EINTR);

            if(wait_val == -1)
            {
                throw boost::system::system_error(boost::system::error_code(errno, boost::system::system_category()));
            }
            
            return status;
        }

        template<class EXC> void join() const
        {
            if(join()) throw EXC();
        }

        void terminate() const
        {
            if(kill(m_id, SIGKILL) == -1)    //: SIGTERM
            {
                throw boost::system::system_error(boost::system::error_code(errno, boost::system::system_category()));
            }
        }
        
        pid_t m_id;
        
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

#endif // BOOST_PROCESS_POSIX_MONITOR_HPP
