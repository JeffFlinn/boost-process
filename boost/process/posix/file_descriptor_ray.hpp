
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_FILE_DESCRIPTOR_RAY_HPP
#define BOOST_PROCESS_POSIX_FILE_DESCRIPTOR_RAY_HPP

#include <boost/iostreams/device/file_descriptor.hpp>

#include <fcntl.h>

namespace boost { namespace process { namespace posix {

    struct file_descriptor_ray 
    {
        typedef boost::iostreams::file_descriptor_source source_type;
        typedef boost::iostreams::file_descriptor_sink     sink_type;

        file_descriptor_ray() : m_sink(), m_source() 
        {
            int pipe_fds[2] = {-1, -1};

            if(!pipe(pipe_fds))
            {
                m_sink  .open(ensure_user_fd_index(pipe_fds[1]), boost::iostreams::close_handle);
                m_source.open(ensure_user_fd_index(pipe_fds[0]), boost::iostreams::close_handle);
            }
            else
            {
                // throw
            }

        }
        
        static int ensure_user_fd_index(int fd_index)
        {
            //BOOST_STATIC_ASSERT( STDIN_FILENO == 0)
            //BOOST_STATIC_ASSERT(STDOUT_FILENO == 1)
            //BOOST_STATIC_ASSERT(STDERR_FILENO == 2)

            const int user_fileno_begin = 3;

            int user_fd_index = fcntl(fd_index, F_DUPFD, user_fileno_begin);

            close(fd_index);

            return user_fd_index;
        }

          sink_type m_sink;
        source_type m_source;
    };

}}}

#endif // BOOST_PROCESS_POSIX_FILE_DESCRIPTOR_RAY_HPP
