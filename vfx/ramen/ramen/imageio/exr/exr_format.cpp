// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exr/exr_format.hpp>

#include<OpenEXR/ImfVersion.h>

#include<ramen/imageio/factory.hpp>

#include<ramen/imageio/exr/exr_reader.hpp>
#include<ramen/imageio/exr/exr_writer.hpp>

namespace ramen
{
namespace imageio
{

exr_format_t::exr_format_t() {}

std::string exr_format_t::tag() const { return "exr";}

bool exr_format_t::check_extension( const std::string& str) const
{ 
    return str == ".exr" || str == ".EXR";
}

std::size_t exr_format_t::detect_size() const { return 4;}

bool exr_format_t::detect( const char *p) const { return Imf::isImfMagic( p);}

void exr_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "exr");
    ext.push_back( "EXR");
}

std::auto_ptr<reader_t> exr_format_t::reader( const boost::filesystem::path& p) const
{
    std::auto_ptr<reader_t> r( new exr_reader_t( p));
    return r;
}

std::auto_ptr<writer_t> exr_format_t::writer() const
{
    std::auto_ptr<writer_t> w( new exr_writer_t());
    return w;
}

static bool registered = factory_t::Instance().register_image_format( std::auto_ptr<format_t>( new exr_format_t()));

} // namespace
} // namespace
