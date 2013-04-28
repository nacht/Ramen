// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/dpx/dpx_format.hpp>
#include<ramen/imageio/dpx/dpx_reader.hpp>
#include<ramen/imageio/dpx/dpx_writer.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{
namespace imageio
{

dpx_format_t::dpx_format_t() {}

std::string dpx_format_t::tag() const { return std::string( "dpx");}

bool dpx_format_t::check_extension( const std::string& str) const
{ 
    return str == ".dpx" || str == ".DPX";
}

std::size_t dpx_format_t::detect_size() const { return 4;}

bool dpx_format_t::detect( const char *p) const
{
    const boost::uint32_t *q = reinterpret_cast<const boost::uint32_t*>( p);
    return ( *q == 0x53445058) || ( *q == 0x58504453);
}

void dpx_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "dpx");
    ext.push_back( "DPX");
}

std::auto_ptr<reader_t> dpx_format_t::reader( const boost::filesystem::path& p) const
{
    std::auto_ptr<reader_t> r( new dpx_reader_t( p));
    return r;
}

std::auto_ptr<writer_t> dpx_format_t::writer() const
{
    std::auto_ptr<writer_t> w( new dpx_writer_t());
    return w;
}

static bool registered = factory_t::Instance().register_image_format( std::auto_ptr<format_t>( new dpx_format_t()));

} // namespace
} // namespace
