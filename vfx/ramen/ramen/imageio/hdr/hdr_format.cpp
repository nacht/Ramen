// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/hdr/hdr_format.hpp>

#include<ramen/imageio/oiio/oiio_reader.hpp>

#include<ramen/imageio/hdr/hdr_writer.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{
namespace imageio
{

hdr_format_t::hdr_format_t() {}
hdr_format_t::~hdr_format_t() {}

std::string hdr_format_t::tag() const { return std::string( "hdr");}

bool hdr_format_t::check_extension( const std::string& str) const
{ 
    return str == ".hdr" || str == ".HDR" || str == ".rgbe" || str == ".RGBE";
}

std::size_t hdr_format_t::detect_size() const
{ 
    //return 18;
    return 0;
}

bool hdr_format_t::detect( const char *p) const
{ 
    // we can do better
    return false;
}

void hdr_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "hdr");
    ext.push_back( "rgbe");
    ext.push_back( "HDR");
    ext.push_back( "RGBE");
}

std::auto_ptr<reader_t> hdr_format_t::reader( const boost::filesystem::path& p) const
{
    std::auto_ptr<reader_t> r( new oiio_reader_t( p));
    return r;
}

std::auto_ptr<writer_t> hdr_format_t::writer() const
{
    std::auto_ptr<writer_t> w( new hdr_writer_t());
    return w;
}

static bool registered = factory_t::Instance().register_image_format( std::auto_ptr<format_t>( new hdr_format_t()));

} // namespace
} // namespace
