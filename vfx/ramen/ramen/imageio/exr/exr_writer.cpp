// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exr/exr_writer.hpp>

#include<gil/extension/half.hpp>

#include<OpenEXR/ImfOutputFile.h>
#include<OpenEXR/ImfChannelList.h>

namespace ramen
{
namespace imageio
{

void exr_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    int channels    = adobe::get_value( params, adobe::name_t( "channels")).cast<int>();
    int type	    = adobe::get_value( params, adobe::name_t( "type")).cast<int>();
    int compress    = adobe::get_value( params, adobe::name_t( "compress")).cast<int>();
    bool multilevel = adobe::get_value( params, adobe::name_t( "multilevel")).cast<bool>();

    Imf::Header header( view.width(), view.height());

    switch( compress)
    {
    case none_comp:
	header.compression() = Imf::NO_COMPRESSION;
    break;

    case rle_comp:
        header.compression() = Imf::RLE_COMPRESSION;
    break;

    case zips_comp:
	header.compression() = Imf::ZIPS_COMPRESSION;
    break;

    case zip_comp:
	header.compression() = Imf::ZIP_COMPRESSION;
    break;

    case piz_comp:
	header.compression() = Imf::PIZ_COMPRESSION;
    break;

    case pxr24_comp:
	header.compression() = Imf::PXR24_COMPRESSION;
    break;

    case b44_comp:
	header.compression() = Imf::B44_COMPRESSION;
    break;

    case b44a_comp:
	header.compression() = Imf::B44A_COMPRESSION;
    break;
    }

    //if( !multilevel)
    if( 1)
    {
	if( type == half_channel)
	{
	    if( channels == alpha_channel)
		write_half_alpha( p, header, view);
	    else
		write_half( p, header, view, channels == rgba_channels);
	}
	else
	{
	    if( channels == alpha_channel)
		write_float_alpha( p, header, view);
	    else
		write_float( p, header, view, channels == rgba_channels);
	}
    }
    else
    {
	assert( 0);

	if( type == half_channel)
	{
	    if( channels == alpha_channel)
		write_multilevel_half_alpha( p, header, view);
	    else
		write_multilevel_half( p, header, view, channels == rgba_channels);
	}
	else
	{
	    if( channels == alpha_channel)
		write_multilevel_float_alpha( p, header, view);
	    else
		write_multilevel_float( p, header, view, channels == rgba_channels);
	}
    }
}

void exr_writer_t::write_half( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha) const
{
    boost::gil::rgba16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( view, boost::gil::view( img));

    header.channels().insert( "R", Imf::HALF);
    header.channels().insert( "G", Imf::HALF);
    header.channels().insert( "B", Imf::HALF);

    if( save_alpha)
        header.channels().insert( "A", Imf::HALF);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = 4 * sizeof(half);
    std::size_t ystride = xstride * img.width();

    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);

    if( save_alpha)
        frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr, xstride, ystride));

    Imf::OutputFile out_file( p.external_file_string().c_str(), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

void exr_writer_t::write_half_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const
{
    boost::gil::gray16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( boost::gil::nth_channel_view( view, 3), boost::gil::view( img));

    header.channels().insert( "A", Imf::HALF);
    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = sizeof(half);
    std::size_t ystride = xstride * img.width();

    frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr, xstride, ystride));

    Imf::OutputFile out_file( p.external_file_string().c_str(), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

void exr_writer_t::write_float( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha) const
{
    header.channels().insert( "R", Imf::FLOAT);
    header.channels().insert( "G", Imf::FLOAT);
    header.channels().insert( "B", Imf::FLOAT);

    if( save_alpha)
        header.channels().insert( "A", Imf::FLOAT);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    frameBuffer.insert( "R", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);
    frameBuffer.insert( "G", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);
    frameBuffer.insert( "B", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);

    if( save_alpha)
        frameBuffer.insert( "A", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride));

    Imf::OutputFile out_file( p.external_file_string().c_str(), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( view.height());
}

void exr_writer_t::write_float_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const
{
    header.channels().insert( "A", Imf::FLOAT);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    ptr += 3 * sizeof( float);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    frameBuffer.insert( "A", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride));

    Imf::OutputFile out_file( p.external_file_string().c_str(), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( view.height());
}

void exr_writer_t::write_multilevel_half( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha) const
{
}

void exr_writer_t::write_multilevel_half_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const
{
}

void exr_writer_t::write_multilevel_float( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha) const
{
}

void exr_writer_t::write_multilevel_float_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const
{
}

} // namespace
} // namespace
