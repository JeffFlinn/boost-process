
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_FILE_DESCRIPTOR_RAY_HPP
#define BOOST_PROCESS_WINDOWS_FILE_DESCRIPTOR_RAY_HPP

#include <boost/iostreams/device/file_descriptor.hpp>

namespace boost { namespace process { namespace windows {

    struct file_descriptor_ray 
    {
        typedef boost::iostreams::file_descriptor_source source_type;
        typedef boost::iostreams::file_descriptor_sink     sink_type;

        file_descriptor_ray() : m_sink(), m_source() 
        {
            SECURITY_ATTRIBUTES sa = {0};

            sa.nLength              = sizeof(sa);
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle       = FALSE;
            
            HANDLE source_hdl;
            HANDLE   sink_hdl;
            
            if (::CreatePipe(&source_hdl, &sink_hdl, &sa, 0))
            {
                m_source.open(source_hdl, boost::iostreams::close_handle);
                m_sink  .open(  sink_hdl, boost::iostreams::close_handle);
            }
            else
            {
                throw boost::system::system_error(boost::system::error_code(::GetLastError(), boost::system::system_category()));
            }
        }
        
          sink_type m_sink;
        source_type m_source;
    };

}}}

#endif
