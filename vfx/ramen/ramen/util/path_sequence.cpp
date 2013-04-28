// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
#include<ramen/util/path_sequence.hpp>

#include<algorithm>

#include<boost/tokenizer.hpp>
#include<boost/lexical_cast.hpp>

#include<iostream>

namespace ramen
{

path_sequence_t::path_sequence_t()
{
    start_ = end_ = pad_ = 0;
    is_seq_ = false;
}

path_sequence_t::path_sequence_t( const boost::filesystem::path& p, bool sequence)
{
    dir_ = p.branch_path();
    std::string str = p.leaf();

    boost::tuple<std::string, std::string, std::string> parts( decompose_filename( str));

    if( sequence)
    {
	base_ = boost::get<0>( parts);
	ext_ = boost::get<2>( parts);

	start_ = end_ = 0;
	pad_ = 1;

	std::string fnum = boost::get<1>( parts);
	is_seq_ = !fnum.empty();

	if( is_seq_)
	{
	    pad_ = get_pad( fnum);
	    start_ = boost::lexical_cast<int>( fnum);
	    end_ = start_;
	}
    }
    else
    {
	is_seq_ = false;
	ext_ = boost::get<2>( parts);
	base_ = std::string( str, 0, str.size() - ext_.size());
    	start_ = end_ = 0;
	pad_ = 1;
    }
}

boost::filesystem::path path_sequence_t::operator()( int n) const
{
    if( !is_seq_)
        return dir_ / ( base_ + ext_);

    std::string num_str = boost::lexical_cast<std::string>( n);

    while( num_str.length() < pad_)
            num_str.insert( 0, "0");

    std::string filename = base_ + num_str + ext_;
    return dir_ / filename;
}

std::string path_sequence_t::format_string() const
{
    if( !is_seq_)
       return dir_.string() + "/" + base_ + ext_;
    else
    {
        std::string fnum;

        if( pad_ != 1)
        {
            fnum = "%0";
            fnum += boost::lexical_cast<std::string>( pad_);
            fnum += "d";
        }
        else
            fnum = "%d";

        return dir_.string() + "/" + base_ + fnum + ext_;
    }

}

bool path_sequence_t::add_path( const boost::filesystem::path& p)
{
    if( !is_seq_)
        return false;

    boost::filesystem::path new_dir = p.branch_path();

    if( new_dir.string() != dir_.string())
        return false;

    std::string str = p.leaf();
    boost::tuple<std::string, std::string, std::string> parts( decompose_filename( str));
        
    std::string new_base = boost::get<0>( parts);

    if( base_.empty())
    {
	if( !new_base.empty())
	    return false;
    }
    else
    {
	if( base_ != new_base)
	    return false;
    }
    
    std::string new_ext = boost::get<2>( parts);
    
    if( ext_ != new_ext)
	return false;

    std::string fnum( boost::get<1>( parts));

    if( fnum.empty())
	return false;

    int n = boost::lexical_cast<int>( fnum);
    start_ = std::min( start_, n);
    end_ = std::max( end_, n);

    pad_ = std::max( pad_, get_pad( fnum));
    return true;
}

std::string path_sequence_t::get_extension( const std::string& str) const
{
    std::string::size_type n = str.find_last_of( ".");

    if( n != std::string::npos)
    {
	std::string ext( std::string( str, n, std::string::npos));

    	if( !ext.empty())
    	{
	    for( int i=1;i<ext.length();++i)
	    {
		if( !isdigit( ext[i]))
		    return ext;
	    }
	}
    }

    return std::string();
}

std::string path_sequence_t::get_frame_number( const std::string& str) const
{
    try
    {
	// this is a quick fix, if all str are nums
	int num = boost::lexical_cast<int>( str);
	return str;
    }
    catch( ...) {}

    std::string::size_type n = str.length() - 1;

    while( n != 0)
    {
	char c = str[n];

	if( !isdigit( c))
		break;

	--n;
    }

    return std::string( str, n + 1, str.length());
}

int path_sequence_t::get_pad( const std::string& str) const
{
    if( str[0] == '0')
	return str.length();

    return 1;
}

boost::tuple<std::string, std::string, std::string> path_sequence_t::decompose_filename( const std::string& filename) const
{
    std::string ext  = get_extension( filename);
    std::string fnum = get_frame_number( std::string( filename, 0, filename.length() - ext.length()));

    std::string name;
    int name_lenght = filename.length() - fnum.length() - ext.length();

    if( name_lenght > 0)
	name = std::string( filename, 0, name_lenght);

    return boost::make_tuple( name, fnum, ext);
}

// TODO: implement this
bool path_sequence_t::operator==( const path_sequence_t& other) const { return false;}
bool path_sequence_t::operator!=( const path_sequence_t& other) const { return true;}

std::ostream& operator << ( std::ostream& os, const path_sequence_t& p)
{
    os << p.dir_.string() << "\n";
    os << p.base_ << "\n";
    os << p.ext_ << "\n";

    if( p.is_sequence())
        os << "[" << p.start_ << ", " << p.end_ << ", " << p.pad_ << "]\n";
    else
	os << "single frame\n";

    return os;
}

} // namespace

