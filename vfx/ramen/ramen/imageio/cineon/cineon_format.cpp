// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/cineon/cineon_format.hpp>
#include<ramen/imageio/cineon/cineon_reader.hpp>
#include<ramen/imageio/cineon/cineon_writer.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{
namespace imageio
{

cineon_format_t::cineon_format_t() {}

std::string cineon_format_t::tag() const { return std::string( "cin");}

bool cineon_format_t::check_extension( const std::string& str) const
{ 
    return str == ".cin" || str == ".CIN";
}

std::size_t cineon_format_t::detect_size() const { return 4;}

bool cineon_format_t::detect( const char *p) const
{
    const boost::uint32_t *q = reinterpret_cast<const boost::uint32_t*>( p);
    return ( *q == 0x802A5FD7) || ( *q == 0xD75F2A80);
}

void cineon_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "cin");
    ext.push_back( "CIN");
}

std::auto_ptr<reader_t> cineon_format_t::reader( const boost::filesystem::path& p) const
{
    std::auto_ptr<reader_t> r( new cineon_reader_t( p));
    return r;
}

std::auto_ptr<writer_t> cineon_format_t::writer() const
{
    std::auto_ptr<writer_t> w( new cineon_writer_t());
    return w;
}

static bool registered = factory_t::Instance().register_image_format( std::auto_ptr<format_t>( new cineon_format_t()));

} // namespace
} // namespace
