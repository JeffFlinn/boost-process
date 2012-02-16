
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_STD_IO_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_STD_IO_HPP

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/optional.hpp>
#include <boost/process/file_descriptor_ray.hpp>

namespace boost { namespace process { namespace windows {

    struct io_initializer : public initializer 
    {
        typedef boost::filesystem::path                         path;
        typedef boost::iostreams::file_descriptor_source source_type;
        typedef boost::iostreams::file_descriptor_sink     sink_type;
    };

//////////////////////////////////////////////////////////////////////////
    
    struct std_in_from_path : io_initializer
    {
        std_in_from_path(const path& p) : io_initializer(), m_path(p), m_source() {}

        template<class Executor> void pre_create(Executor& e) const
        {
            m_source = source_type(m_path);
            
            e.set_std_in((*m_source).handle());
        }
        
        template<class Executor> void post_create(Executor&) const
        {
            m_source.reset();
        }
        
        path                                 m_path;
        mutable boost::optional<source_type> m_source;
    };
    
    struct std_out_to_path : io_initializer
    {
        std_out_to_path(const path& p) : io_initializer(), m_path(p), m_sink() {}

        template<class Executor> void pre_create(Executor& e) const
        {
            m_sink = sink_type(m_path);

            e.set_std_out((*m_sink).handle());
        }

        template<class Executor> void post_create(Executor&) const
        {
            m_sink.reset();
        }

        path                               m_path;
        mutable boost::optional<sink_type> m_sink;
    };

    struct std_err_to_path : io_initializer
    {
        std_err_to_path(const path& p) : io_initializer(), m_path(p), m_sink() {}

        template<class Executor> void pre_create(Executor& e) const
        {
            m_sink = sink_type(m_path);

            e.set_std_err((*m_sink).handle());
        }

        template<class Executor> void post_create(Executor&) const
        {
            m_sink.reset();
        }

        path                               m_path;
        mutable boost::optional<sink_type> m_sink;
    };

//////////////////////////////////////////////////////////////////////////

    struct std_in_from : io_initializer
    {
        std_in_from(const         source_type& s) : io_initializer(), m_source(s         ) {}
        std_in_from(const file_descriptor_ray& r) : io_initializer(), m_source(r.m_source) {}

        template<class Executor> void pre_create(Executor& e) const
        {
            e.set_std_in(m_source.handle());
        }

        const source_type& m_source;
    };

    struct std_out_to : io_initializer
    {
        std_out_to(const           sink_type& s) : io_initializer(), m_sink(s       ) {}
        std_out_to(const file_descriptor_ray& r) : io_initializer(), m_sink(r.m_sink) {}

        template<class Executor> void pre_create(Executor& e) const
        {
            e.set_std_out(m_sink.handle());
        }

        const sink_type& m_sink;
    };

    struct std_err_to : io_initializer
    {
        std_err_to(const           sink_type& s) : io_initializer(), m_sink(s       ) {}
        std_err_to(const file_descriptor_ray& r) : io_initializer(), m_sink(r.m_sink) {}

        template<class Executor> void pre_create(Executor& e) const
        {
            e.set_std_err(m_sink.handle());
        }

        const sink_type& m_sink;
    };

}}}

#endif
