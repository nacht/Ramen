// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/tiff/tiff_writer.hpp>

#include<memory>
#include<vector>
#include<algorithm>

namespace ramen
{
namespace imageio
{

void tiff_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    int channels = adobe::get_value( params, adobe::name_t( "channels")).cast<int>();
    int depth	 = adobe::get_value( params, adobe::name_t( "type")).cast<int>();
    int compress = adobe::get_value( params, adobe::name_t( "compress")).cast<int>();

    std::auto_ptr<OpenImageIO::ImageOutput> out( OpenImageIO::ImageOutput::create( p.external_file_string()));

    if( !out.get())
	throw std::runtime_error( "Write TIFF: Can't open output file");

    if( channels)
	channels = 3;
    else
	channels = 4;

    OpenImageIO::ImageSpec spec;

    switch( depth)
    {
    case 1:
	spec = OpenImageIO::ImageSpec( view.width(), view.height(), channels, TypeDesc::UINT8);
    break;

    case 2:
	spec = OpenImageIO::ImageSpec( view.width(), view.height(), channels, TypeDesc::UINT16);
    break;

    case 3:
	spec = OpenImageIO::ImageSpec( view.width(), view.height(), channels, TypeDesc::FLOAT);
    break;
    }

    spec.quant_dither = 0.0f;

    switch( compress)
    {
    case 0:
	spec.attribute( "compress", "none");
    break;

    case 1:
	spec.attribute( "compress", "lzw");
    break;

    case 2:
	spec.attribute( "compress", "zip");
    break;
    }

    if( !out->open( p.external_file_string(), spec))
	throw( std::runtime_error( "Can't open output file"));

    std::vector<image::pixel_t> scanline( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
	std::copy( view.row_begin( y), view.row_end( y), scanline.begin());

	if( depth != 3)
	{
	    apply_gamma( 1.0f / 2.2f, scanline.begin(), scanline.end());
	    clamp( scanline.begin(), scanline.end());
	}

	if( !out->write_scanline( y, 0, TypeDesc::FLOAT, (void *) &( *scanline.begin()), sizeof( image::pixel_t)))
	    throw( std::runtime_error( "Write image: Can't write pixels"));
    }

    if( !out->close())
	throw std::runtime_error( "Write image: Can't close file");
}
    
} // namespace
} // namespace
