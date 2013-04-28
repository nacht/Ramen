// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/png/png_writer.hpp>

#include<vector>
#include<algorithm>

namespace ramen
{
namespace imageio
{

void png_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    int channels = adobe::get_value( params, adobe::name_t( "channels")).cast<int>();

    std::auto_ptr<OpenImageIO::ImageOutput> out( OpenImageIO::ImageOutput::create( p.external_file_string()));

    if( !out.get())
	throw std::runtime_error( "Write PNG: Can't open output file");

    if( channels)
	channels = 3;
    else
	channels = 4;

    OpenImageIO::ImageSpec spec( view.width(), view.height(), channels, TypeDesc::UINT8);
    spec.quant_dither = 0.0f;

    if( !out->open( p.external_file_string(), spec))
	throw( std::runtime_error( "Can't open output file"));
    
    std::vector<image::pixel_t> scanline( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
	std::copy( view.row_begin( y), view.row_end( y), scanline.begin());
	apply_gamma( 1.0f / 2.2f, scanline.begin(), scanline.end());
	clamp( scanline.begin(), scanline.end());

	if( !out->write_scanline( y, 0, TypeDesc::FLOAT, (void *) &( *scanline.begin()), sizeof( image::pixel_t)))
	    throw( std::runtime_error( "Write image: Can't write pixels"));
    }

    if( !out->close())
	throw std::runtime_error( "Write image: Can't close file");
}
    
} // namespace
} // namespace
