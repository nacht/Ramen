// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/read_exr.hpp>

#include<algorithm>

#include<OpenEXR/ImfInputFile.h>
#include<OpenEXR/ImfTiledInputFile.h>

#include<OpenEXR/ImfChannelList.h>
#include<OpenEXR/ImfRgbaFile.h>
#include<OpenEXR/ImfArray.h>

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{

using namespace image;

namespace imageio
{

namespace
{

void copy_scanline( const image::image_view_t& view, unsigned int src_line, unsigned int dst_line)
{
    assert( src_line >= 0);
    assert( src_line < view.height());
    assert( dst_line >= 0);
    assert( dst_line < view.height());
    assert( src_line != dst_line);

    std::copy( view.row_begin( src_line), view.row_end( src_line), view.row_begin( dst_line));
}

void copy_subsampled_scanline( image_view_t::x_iterator src_begin, image_view_t::x_iterator src_end,
                                image_view_t::x_iterator dst_begin, image_view_t::x_iterator dst_end, int subsample)
{
    image_view_t::x_iterator src_it = src_begin;
    image_view_t::x_iterator dst_it = dst_begin;

    while( src_it < src_end)
    {
        if( dst_it == dst_end)
            return;

        *dst_it++ = *src_it;
        src_it += subsample;
    }

    // repeat the last pixel until we fill the scan line
    while( dst_it < dst_end)
        *dst_it++ = *(src_end - 1);
}

void copy_and_convert_subsampled_scanline( image_view_t::x_iterator src_begin, image_view_t::x_iterator src_end,
                                image_view_t::x_iterator dst_begin, image_view_t::x_iterator dst_end, int subsample)
{
    image_view_t::x_iterator src_it = src_begin;
    image_view_t::x_iterator dst_it = dst_begin;

    while( src_it < src_end)
    {
        boost::gil::color_convert( *src_it, *dst_it++);
        src_it += subsample;
    }

    // repeat the last pixel until we fill the scan line
    while( dst_it < dst_end)
        boost::gil::color_convert( *(src_end - 1), *dst_it++);
}

} // unnamed

void read_exr_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
		    const Imath::Box2i& crop, std::size_t subsample)
{
    read_exr_image( p, result_view, crop, "R", "G", "B", "A", subsample);
}

void read_exr_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
		    const Imath::Box2i& crop, const char *rchannel, const char *gchannel,
		    const char *bchannel, const char *achannel, std::size_t subsample)
{
    Imf::InputFile file( p.external_file_string().c_str());
    Imath::Box2i data_window( file.header().dataWindow());

    std::size_t width  = data_window.max.x - data_window.min.x + 1;
    std::size_t xstride = 4 * sizeof(float);
    std::size_t ystride = width * xstride;

    boost::gil::rgba32f_image_t scan_line( width, 1);
    boost::gil::fill_pixels( boost::gil::view( scan_line), image::pixel_t( 0, 0, 0, 1));
    float *buffer = (float *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( scan_line));

    assert( result_view.width() >= std::ceil( (float) (crop.size().x+1) / subsample));
    assert( result_view.height() >= std::ceil( (float) (crop.size().y+1) / subsample));

    Imf::FrameBuffer frameBuffer;

    int dst_y = 0;
    for( int y = crop.min.y; y <= crop.max.y; y += subsample)
    {
	char *ptr = ((char *) buffer) - (y * ystride) - (data_window.min.x * xstride);

	if( rchannel)
	    frameBuffer.insert( rchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

	ptr += sizeof( float);

	if( gchannel)
	    frameBuffer.insert( gchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

	ptr += sizeof( float);

	if( bchannel)
	    frameBuffer.insert( bchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

	ptr += sizeof( float);

	if( achannel)
	    frameBuffer.insert( achannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride, 1, 1, 1.0));

	file.setFrameBuffer( frameBuffer);
	file.readPixels( y);

	image_view_t::x_iterator dst_begin( result_view.row_begin( dst_y));
	image_view_t::x_iterator dst_end( result_view.row_end( dst_y));

	image_view_t::x_iterator src_begin( boost::gil::view( scan_line).row_begin( 0) + crop.min.x - data_window.min.x);
        image_view_t::x_iterator src_end( src_begin + crop.size().x + 1);

        copy_subsampled_scanline( src_begin, src_end, dst_begin, dst_end, subsample);
	++dst_y;
    }

    int last_scan = dst_y - 1;

    while( dst_y < result_view.height())
    {
        copy_scanline( result_view, last_scan, dst_y);
        ++dst_y;
    }
}

void read_exr_luma_chroma_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
				    const Imath::Box2i& crop, std::size_t subsample)
{
    // TODO: optimize this, read only scanlines needed
    assert( result_view.width() >= std::ceil( (float) (crop.size().x+1) / subsample));
    assert( result_view.height() >= std::ceil( (float) (crop.size().y+1) / subsample));

    Imf::RgbaInputFile file( p.external_file_string().c_str());
    Imath::Box2i data_window( file.header().dataWindow());

    std::size_t width  = data_window.max.x - data_window.min.x + 1;
    std::size_t height = data_window.max.y - data_window.min.y + 1;

    boost::gil::rgba16f_image_t buffer( width, height);
    Imf::Rgba *ptr = (Imf::Rgba *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( buffer));

    file.setFrameBuffer( ptr - data_window.min.x - (data_window.min.y * width), 1, width);
    
    file.readPixels( data_window.min.y, data_window.max.y);

    boost::gil::rgba16fc_view_t src_view( boost::gil::const_view( buffer));

    for( int j = 0; j < result_view.height(); ++j)
    {
	int src_y = crop.min.y - data_window.min.y + (j * subsample);

	if( src_y >= src_view.height())
	    return;
	
	boost::gil::rgba16fc_view_t::x_iterator src_it  = src_view.row_begin( src_y);
	src_it += crop.min.x - data_window.min.x;

	boost::gil::rgba16fc_view_t::x_iterator src_end  = src_view.row_begin( src_y);
	src_end += crop.max.x - data_window.min.x + 1;

	image_view_t::x_iterator dst_it( result_view.row_begin( j));

	for( int i = 0; i < result_view.width(); ++i)
	{
	    if( src_it >= src_end)
		break;

	    boost::gil::color_convert( *src_it, *dst_it++);
	    src_it += subsample;
	}
    }
}

void read_exr_image( Imf::IStream& is, boost::gil::rgba16f_image_t& image)
{
    Imf::InputFile file( is);
    Imath::Box2i dw( file.header().dataWindow());

    int width  = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    if( ( image.width() != width) || ( image.height() != height))
	image.recreate( width, height);

    boost::gil::rgba16f_view_t view( boost::gil::view( image));

    std::size_t xstride = 4 * sizeof( half);
    std::size_t ystride = width * xstride;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);

    Imf::FrameBuffer frameBuffer;

    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr , xstride, ystride, 1, 1, 1.0));

    file.setFrameBuffer( frameBuffer);
    file.readPixels( dw.min.y, dw.max.y);
}

void read_exr_image_proxy_third( const boost::filesystem::path& p, const image::image_view_t& result_view,
			    const Imath::Box2i& crop, const char *rchannel, const char *gchannel,
			    const char *bchannel, const char *achannel)
{
    int level = 1;

    Imf::TiledInputFile file( p.external_file_string().c_str());
    Imath::Box2i data_window( file.header().dataWindow());

    boost::gil::rgba32f_image_t buffer( file.levelWidth( level), file.levelHeight( level));
    boost::gil::fill_pixels( boost::gil::view( buffer), image::pixel_t( 0, 0, 0, 1));

    std::size_t width = file.levelWidth( level);
    std::size_t xstride = 4 * sizeof(float);
    std::size_t ystride = width * xstride;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( buffer));
    ptr = ptr - ( data_window.min.y * ystride) - ( data_window.min.x * xstride);

    Imf::FrameBuffer frameBuffer;

    if( rchannel)
	frameBuffer.insert( rchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));
    
    ptr += sizeof( float);

    if( gchannel)
	frameBuffer.insert( gchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

    ptr += sizeof( float);

    if( bchannel)
	frameBuffer.insert( bchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

    ptr += sizeof( float);

    if( achannel)
	frameBuffer.insert( achannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride, 1, 1, 1.0));

    file.setFrameBuffer( frameBuffer);
    file.readTiles( 0, file.numXTiles( level) - 1, 0, file.numYTiles( level) - 1, level);

    image::const_image_view_t src( boost::gil::subimage_view( boost::gil::const_view( buffer),
							     ( crop.min.x - data_window.min.x) / 2,
                                                             ( crop.min.y - data_window.min.y) / 2,
							     crop.size().x / 2, crop.size().y / 2));

    // TODO: replace this by the new subsample routines in processing
    // but this would add a dependency, in this is a special case not very frequent...
    image::generic::bilinear_sampler_t sampler( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( src.width(), src.height())), src);
    Imath::V2f q;

    float sx = (float) src.width()  / result_view.width();
    float sy = (float) src.height() / result_view.height();

    for( int y = 0; y < result_view.height(); ++y)
    {
	for( int x = 0; x < result_view.width(); ++x)
	{
	    q.x = x * sx;
	    q.y = y * sy;
	    result_view( x, y) = sampler( q);
	}
    }
}

void read_exr_image_proxy( const boost::filesystem::path& p, const image::image_view_t& result_view,
			    const Imath::Box2i& crop, std::size_t subsample)
{
    read_exr_image_proxy( p, result_view, crop, "R", "G", "B", "A", subsample);
}

void read_exr_image_proxy( const boost::filesystem::path& p, const image::image_view_t& result_view,
			    const Imath::Box2i& crop, const char *rchannel, const char *gchannel,
			    const char *bchannel, const char *achannel, std::size_t subsample)
{
    if( subsample == 3)
    {
	read_exr_image_proxy_third( p, result_view, crop, rchannel, gchannel, bchannel, achannel);
	return;
    }

    int level;

    if( subsample == 2)
	level = 1;
    else
	level = 2; // quarter
    
    Imf::TiledInputFile file( p.external_file_string().c_str());
    Imath::Box2i data_window( file.header().dataWindow());

    boost::gil::rgba32f_image_t buffer( file.levelWidth( level), file.levelHeight( level));
    boost::gil::fill_pixels( boost::gil::view( buffer), image::pixel_t( 0, 0, 0, 1));

    std::size_t width = file.levelWidth( level);
    std::size_t xstride = 4 * sizeof(float);
    std::size_t ystride = width * xstride;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( buffer));
    ptr = ptr - ( data_window.min.y * ystride) - ( data_window.min.x * xstride);

    Imf::FrameBuffer frameBuffer;

    if( rchannel)
	frameBuffer.insert( rchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

    ptr += sizeof( float);

    if( gchannel)
	frameBuffer.insert( gchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

    ptr += sizeof( float);

    if( bchannel)
	frameBuffer.insert( bchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

    ptr += sizeof( float);

    if( achannel)
	frameBuffer.insert( achannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride, 1, 1, 1.0));

    file.setFrameBuffer( frameBuffer);
    file.readTiles( 0, file.numXTiles( level) - 1, 0, file.numYTiles( level) - 1, level);
    
    // copy to result
    int ystart = ( crop.min.y - data_window.min.y) / subsample;	// TODO: check this
    int yend = ystart + std::min( result_view.height(), buffer.height());
    int dst_y = 0;

    for( int y = ystart; y < yend; ++y)
    {

	image_view_t::x_iterator src_it( boost::gil::view( buffer).row_begin( y));
        src_it += ( crop.min.x - data_window.min.x) / subsample;

	image_view_t::x_iterator src_end( boost::gil::view( buffer).row_end( y));

        copy_subsampled_scanline( src_it, src_end, result_view.row_begin( dst_y), result_view.row_end( dst_y), 1);
        ++dst_y;
    }

    int last_scan = dst_y - 1;

    while( dst_y < result_view.height())
    {
        copy_scanline( result_view, last_scan, dst_y);
        ++dst_y;
    }
}

} // namespace
} // namespace
