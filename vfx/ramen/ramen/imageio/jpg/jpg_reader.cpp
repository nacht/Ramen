// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/jpg/jpg_reader.hpp>
#include<ramen/imageio/jpg/jpg_size.hpp>

#include<ramen/imageio/jpg/jpg_memory.hpp>

namespace ramen
{
namespace imageio
{

jpg_reader_t::jpg_reader_t( const boost::filesystem::path& p) : reader_t( p)
{
    int width, height;
    if( !getJPEGSize( p.external_file_string().c_str(), width, height ))
	throw std::runtime_error( "Can't read header");

    info_[adobe::name_t( "domain")] = adobe::any_regular_t( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width-1, height-1)));

    // TODO: check this
    info_[adobe::name_t( "gamma")] = adobe::any_regular_t( 2.2);
}

void jpg_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error( &jerr);
    jpeg_create_decompress( &cinfo);

    adobe::auto_ptr<FILE> fp( fopen( path_.external_file_string().c_str(), "rb"));

    if ( !fp)
    {
	jpeg_destroy_decompress( &cinfo);
	throw std::runtime_error( "Can't open file");
	return;
    }

    jpeg_stdio_src( &cinfo, fp.get());
    jpeg_read_header( &cinfo, TRUE);
    jpeg_start_decompress( &cinfo);

    if( ( cinfo.output_components != 3) && ( cinfo.output_components != 1))
    {
	jpeg_finish_decompress( &cinfo);
	jpeg_destroy_decompress( &cinfo);
	throw unsupported_image_error();
    }

    std::size_t stride = cinfo.output_width * cinfo.output_components;
    std::vector<boost::uint8_t> scanline_buffer( stride);

    std::vector<boost::uint8_t>::iterator first_item( scanline_buffer.begin());
    unsigned char *ptr = &(*first_item);

    // skip first lines
    int y = 0;
    for( ; y < crop.min.y; ++y)
        jpeg_read_scanlines( &cinfo, &ptr, 1);

    int yy = 0;

    while( y <= crop.max.y)
    {
        jpeg_read_scanlines( &cinfo, &ptr, 1);

	// process one scanline
	image::image_view_t::x_iterator dst_it( view.row_begin( yy));
	unsigned char *q = ptr + ( crop.min.x * cinfo.output_components);
	unsigned char *q_end = ptr + ( ( crop.max.x + 1) * cinfo.output_components);

	if( cinfo.output_components == 3)
	{
	    for( ; dst_it != view.row_end( yy); ++dst_it)
	    {
		boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0] / 255.0f;
		boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[1] / 255.0f;
		boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[2] / 255.0f;
		boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
		q += ( cinfo.output_components * subsample);

		if( q >= q_end)
		    q = ptr + ( crop.max.x * cinfo.output_components);
	    }
	}
	else
	{
	    for( ; dst_it != view.row_end( yy); ++dst_it)
	    {
		float x = *q / 255.0f;
		boost::gil::get_color( *dst_it, boost::gil::red_t())	= x;
		boost::gil::get_color( *dst_it, boost::gil::green_t())	= x;
		boost::gil::get_color( *dst_it, boost::gil::blue_t())	= x;
		boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
		q += ( cinfo.output_components * subsample);

		if( q >= q_end)
		    q = ptr + ( crop.max.x * cinfo.output_components);
	    }
	}

	++yy;

	// skip subsampled scanlines
	for( int s = 0; s < subsample-1; ++s)
	{
            if( cinfo.output_scanline < cinfo.output_height)
                jpeg_read_scanlines( &cinfo, &ptr, 1);

            ++y;
	}

	++y;
    }

    repeat_scanline_until_end( view, yy - 1);

    // skip remaining scanlines
    for( ; cinfo.output_scanline < cinfo.output_height; ++y)
        jpeg_read_scanlines( &cinfo, &ptr, 1);

    jpeg_finish_decompress( &cinfo);
    jpeg_destroy_decompress( &cinfo);
}

} // namespace
} // namespace
