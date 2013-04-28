// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/oiio/oiio_reader.hpp>

#include<boost/scoped_array.hpp>

#include<OpenImageIO/imageio.h>
#include<OpenImageIO/typedesc.h>

#include<ramen/imageio/algorithm.hpp>

namespace ramen
{
namespace imageio
{

oiio_reader_t::oiio_reader_t( const boost::filesystem::path& p, float default_gamma) : reader_t( p), gamma_( default_gamma)
{
    std::auto_ptr<OpenImageIO::ImageInput> in( OpenImageIO::ImageInput::create( p.external_file_string().c_str()));

    if( in.get())
    {
	OpenImageIO::ImageSpec spec;

	if( in->open( p.external_file_string().c_str(), spec))
	{
	    info_[adobe::name_t( "domain")] = adobe::any_regular_t( Imath::Box2i( Imath::V2i( 0, 0),
                                                                                  Imath::V2i( spec.width - 1, spec.height - 1)));

	    if( spec.linearity == OpenImageIO::ImageSpec::sRGB)
		info_[adobe::name_t( "sRGB")] = adobe::any_regular_t( true);
	    else
	    {
		if( spec.linearity == OpenImageIO::ImageSpec::GammaCorrected)
		{
		    // if not gamma info found, we assume 2.2 video standard
		    float gamma = ( spec.gamma != 1.0f) ? spec.gamma : 2.2f;
		    info_[adobe::name_t( "gamma")] = adobe::any_regular_t( gamma);
		}
		else
		{
		    // we use the default gamma
		    info_[adobe::name_t( "gamma")] = adobe::any_regular_t( gamma_);
		}
	    }
	}
	else
	    throw std::runtime_error( "Can't read header");

	in->close();
    }
    else
	throw std::runtime_error( "Can't read header");
}

void oiio_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    std::auto_ptr<OpenImageIO::ImageInput> in( OpenImageIO::ImageInput::create( path_.external_file_string().c_str()));

    if( in.get())
    {
	OpenImageIO::ImageSpec spec;

	if( in->open( path_.external_file_string().c_str(), spec))
	{
	    boost::scoped_array<float> buffer( new float[ spec.width * spec.nchannels]);

            int yy = 0;
	    for( int y = crop.min.y; y <= crop.max.y; y += subsample)
	    {
		in->read_scanline( y, 0, TypeDesc::FLOAT, buffer.get());

		image::image_view_t::x_iterator dst_it( view.row_begin( yy));
		float *q     = buffer.get() + ( crop.min.x * spec.nchannels);
		float *q_end = buffer.get() + (( crop.max.x + 1) * spec.nchannels);
		
		switch( spec.nchannels)
		{
		case 1:
		    for( ; dst_it != view.row_end( yy); ++dst_it)
		    {
			boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
			boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[0];
			boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[0];
			boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
			q += ( spec.nchannels * subsample);

			if( q >= q_end)
			    q = buffer.get() + ( crop.max.x * spec.nchannels);
		    }
		break;

		case 3:
		    for( ; dst_it != view.row_end( yy); ++dst_it)
		    {
			boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
			boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[1];
			boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[2];
			boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
			q += ( spec.nchannels * subsample);

			if( q >= q_end)
			    q = buffer.get() + ( crop.max.x * spec.nchannels);
		    }
		break;

		case 4:
		    for( ; dst_it != view.row_end( yy); ++dst_it)
		    {
			boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
			boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[1];
			boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[2];
			boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= q[3];
			q += ( spec.nchannels * subsample);

			if( q >= q_end)
			    q = buffer.get() + ( crop.max.x * spec.nchannels);
		    }
		break;
		}

                ++yy;
	    }

            repeat_scanline_until_end( view, yy - 1);
	}
	else
	    throw std::runtime_error( "Can't open file");

	in->close();
    }
}

} // namespace
} // namespace
