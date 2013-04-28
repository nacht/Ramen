// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/factory.hpp>

#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/filesystem/fstream.hpp>
#include<ramen/imageio/algorithm.hpp>

namespace ramen
{
namespace imageio
{

factory_impl::factory_impl() : detect_size_(0), detect_buffer_(0) {}
factory_impl::~factory_impl() { delete detect_buffer_;}

bool factory_impl::register_image_format( std::auto_ptr<format_t> format)
{
    detect_size_ = std::max( detect_size_, format->detect_size());
    format->add_extensions( extensions_);
    formats_.push_back( format);
    return true;
}

std::auto_ptr<reader_t> factory_impl::reader_for_image( const boost::filesystem::path& p) const
{
    const_iterator it = format_for_extension( p);

    if( it != formats_.end())
	return it->reader( p);

    it = format_for_file_contents( p);

    if( it != formats_.end())
	return it->reader( p);

    return std::auto_ptr<reader_t>();
}

std::auto_ptr<writer_t> factory_impl::writer_for_tag( const std::string& tag) const
{
    const_iterator it = format_for_tag( tag);

    if( it != formats_.end())
    {
	std::auto_ptr<writer_t> w( it->writer());
	return w;
    }
    else
	return std::auto_ptr<writer_t>();
}

factory_impl::const_iterator factory_impl::format_for_extension( const boost::filesystem::path& p) const
{
    std::string ext( p.extension());

    if( !ext.empty())
    {
        for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
	{
	    if( it->input() &&  it->check_extension( ext))
		return it;
	}
    }

    return formats_.end();
}

factory_impl::const_iterator factory_impl::format_for_file_contents( const boost::filesystem::path& p) const
{
    boost::filesystem::ifstream ifile( p, std::ios::in | std::ios::binary);
    // TODO: detect a problem in ifstream and throw

    if( !detect_buffer_)
	detect_buffer_ = new char[detect_size_];

    ifile.read( detect_buffer_, detect_size_);

    for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
    {
	if( it->input() && it->detect( detect_buffer_))
	    return it;
    }

    return formats_.end();
}

factory_impl::const_iterator factory_impl::format_for_tag( const std::string& tag) const
{
    for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
    {
	if( it->output() && ( it->tag() == tag))
	    return it;
    }

    return formats_.end();
}

} // namespace
} // namespace
