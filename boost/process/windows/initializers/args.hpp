
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_ARGS_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_ARGS_HPP

#include <boost/process/config.hpp>
#include <boost/process/windows/initializers/initializer.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include <algorithm>
#include <ostream>
#include <vector>

namespace boost { namespace process { namespace windows {

    struct arg : public initializer 
    {
        typedef boost::filesystem::path path;
        typedef wchar_t            char_type;
        typedef std::wstring     string_type;

        template<typename T> arg(const           T& t) : m_arg(quoted(boost::lexical_cast<string_type>(t        ))) {}
                             arg(const        path& p) : m_arg(       boost::lexical_cast<string_type>(p        ) ) {}
                             arg(const std::string& s) : m_arg(quoted(boost::lexical_cast<string_type>(s.c_str()))) {}
                             arg(const std::string& s
                                ,const        path& p) : m_arg(quoted(boost::lexical_cast<string_type>(s.c_str())
                                                                     +boost::lexical_cast<string_type>(p.wstring() ))) {}

        template<class Executor> void  pre_create(Executor& e) const 
        { 
            e.m_args.push_back(m_arg); 
        } 

        bool operator==(const arg& rhs) const
        {
            return m_arg == rhs.m_arg;
        }

		static string_type quoted(const string_type& s)
		{
			if (!s.find(L' ') || (!s.empty() && L'"' == s[0] && L'"' == s[s.size()])) return s;

			return L'"' + s + L'"';
		}

		friend std::ostream& operator<<(std::ostream&, const arg&);

		string_type m_arg;
    };

    inline std::ostream& operator<< (std::ostream& os, const arg& a)
    {
        typedef boost:: u16_to_u32_iterator<std::wstring::const_iterator> u32_from_u16_iterator;
        typedef boost::utf8_output_iterator<std::ostream_iterator<char> >  utf8_output_iterator;

        std::copy(u32_from_u16_iterator(a.m_arg.begin()), u32_from_u16_iterator(a.m_arg.end()), utf8_output_iterator(os));

        return os;
    }

	struct args : public initializer 
    {
        typedef std::vector<arg> args_type;

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

        template<class Executor> void  pre_create(Executor& e) const 
        { 
            for_each(m_args.begin(), m_args.end(), initializer::lazy::pre_create<Executor>(e));
        } 

        bool operator==(const args& rhs) const
        {
            return m_args == rhs.m_args;
        }

		friend std::ostream& operator<<(std::ostream&, const args&);

		args_type m_args;
    };

	inline std::ostream& operator<< (std::ostream& os, const args& a)
	{
		std::copy(a.m_args.begin(), a.m_args.end(), std::ostream_iterator<arg>(os, " "));

		return os;
	}

}}}



#endif // BOOST_PROCESS_WINDOWS_INITIALIZER_ARGS_HPP