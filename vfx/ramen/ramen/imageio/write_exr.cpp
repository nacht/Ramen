// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/write_exr.hpp>

#include<gil/extension/half.hpp>

#include<OpenEXR/ImfOutputFile.h>
#include<OpenEXR/ImfChannelList.h>

namespace ramen
{

using namespace image;

namespace imageio
{


void write_half_exr( const boost::filesystem::path& p, Imf::Header& header,
			    const image::const_image_view_t& view, bool write_alpha)
{
    boost::gil::rgba16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( view, boost::gil::view( img));

    header.channels().insert( "R", Imf::HALF);
    header.channels().insert( "G", Imf::HALF);
    header.channels().insert( "B", Imf::HALF);

    if( write_alpha)
        header.channels().insert( "A", Imf::HALF);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = 4 * sizeof(half);
    std::size_t ystride = xstride * img.width();

    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);

    if( write_alpha)
        frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr, xstride, ystride));

    Imf::OutputFile out_file( p.external_file_string().c_str(), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

void write_half_alpha_exr( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view)
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

void write_half_rgb_exr( Imf::OStream& os, Imf::Header& header, const image::const_image_view_t& view)
{
    boost::gil::rgba16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( view, boost::gil::view( img));

    header.channels().insert( "R", Imf::HALF);
    header.channels().insert( "G", Imf::HALF);
    header.channels().insert( "B", Imf::HALF);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = 4 * sizeof(half);
    std::size_t ystride = xstride * img.width();

    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);

    Imf::OutputFile out_file( os, header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

} // namespace
} // namespace
