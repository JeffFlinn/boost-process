
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 
 
#ifndef BOOST_PROCESS_POSIX_INITIALIZER_ARGS_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_ARGS_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <ostream>
#include <vector>

namespace boost { namespace process { namespace posix {

    struct arg : public initializer 
    {
        typedef boost::filesystem::path path;
        typedef std::vector<char>       chars;
        
        mutable chars m_chars;
        
        template<typename T> static chars from(const T& t)
        {
            std::string s = boost::lexical_cast<std::string>(t);
            
            chars val(s.begin(), s.end()); val.push_back(0);
            
            return val;
        }
        
        static chars from(const path& p) { return from(p.string()); }
        
        template<typename T> arg(const           T& t) : m_chars(from(t)) {}
                             arg(const std::string& s
                                ,const        path& p) : m_chars(from(s + p.string())) {}
       
        template<class Executor> void  pre_fork_parent(Executor& e) const 
        { 
            e.m_arg_ptrs.push_back(&m_chars[0]); 
        } 

		bool operator==(const arg& rhs) const
		{
			return m_chars == rhs.m_chars; 
		}
		
		friend std::ostream& operator<< (std::ostream& os, const arg& a);
    };

	inline std::ostream& operator<< (std::ostream& os, const arg& a)
	{
		std::copy(a.m_chars.begin(), boost::prior(a.m_chars.end()), std::ostream_iterator<arg::chars::value_type>(os));
		
		return os;
	}

    struct args : public initializer 
    {
        typedef std::vector<arg> args_type;
        
        args_type m_args;

        args() : m_args() {}
        
        template<typename T> args(const T& t) : m_args(1, arg(t)) {}
        
        args& operator()(const arg& a)
        {
            m_args.push_back(a);
            
            return *this;
        }

        args& operator()(const boost::optional<arg>& a)
        {
            if(a) m_args.push_back(*a);

            return *this;
        }

		bool operator==(const args& rhs) const
		{
			return m_args == rhs.m_args;
		}
		
        template<class Executor> void  pre_fork_parent(Executor& e) const 
        { 
            for_each(m_args.begin(), m_args.end(), lazy::pre_fork_parent<Executor>(e));
        } 

		friend std::ostream& operator<<(std::ostream&, const args&);
    };

	inline std::ostream& operator<< (std::ostream& os, const args& a)
	{
		std::copy(a.m_args.begin(), a.m_args.end(), std::ostream_iterator<arg>(os, " "));

		return os;
	}

}}}

#endif // BOOST_PROCESS_POSIX_INITIALIZER_ARGS_HPP