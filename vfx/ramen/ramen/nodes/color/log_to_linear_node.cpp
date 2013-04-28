// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/log_to_linear_node.hpp>

#include<boost/scoped_array.hpp>
#include<boost/serialization/version.hpp>
#include<boost/cstdint.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<half/halfExpLog.h>
#include<half/half_lut.hpp>

#include<IECore/CineonToLinearDataConversion.h>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float_param.hpp>

// TODO: this node could be optimized and simplified using lookup tables.

namespace ramen
{

namespace
{

enum log2lin_method
{
    cineon_conv = 0,
    redlog_conv,
    exrdpx_conv
};

struct apply_lut_fun
{
    apply_lut_fun( const half_lut_t& lut) : lut_( lut) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	return image::pixel_t( lut_( src[0]), lut_( src[1]), lut_( src[2]), src[3]);
    }

private:

    const half_lut_t& lut_;
};

struct cineon_log_to_linear_fun
{
    cineon_log_to_linear_fun( const IECore::CineonToLinearDataConversion<boost::uint16_t, float>& conv) : conv_( conv) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	using namespace boost::gil;
	boost::uint16_t rlog = adobe::clamp( (float) get_color( src, red_t())  , 0.0f, 1.0f) * 1023.0f;
	boost::uint16_t glog = adobe::clamp( (float) get_color( src, green_t()), 0.0f, 1.0f) * 1023.0f;
	boost::uint16_t blog = adobe::clamp( (float) get_color( src, blue_t()) , 0.0f, 1.0f) * 1023.0f;

        return image::pixel_t(  conv_( rlog), conv_( glog), conv_( blog), get_color( src, alpha_t()));
    }

    const IECore::CineonToLinearDataConversion<boost::uint16_t, float>& conv_;
};

struct cineon_linear_to_log_fun
{
    // TODO: rewrite this to use a lut
    cineon_linear_to_log_fun( const IECore::LinearToCineonDataConversion<float,boost::uint16_t>& conv) : conv_( conv) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	using namespace boost::gil;
	float rlin = get_color( src, red_t());
	float glin = get_color( src, green_t());
	float blin = get_color( src, blue_t());
	return image::pixel_t( conv_( rlin) / 1023.0f, conv_( glin) / 1023.0f, conv_( blin) / 1023.0f, get_color( src, alpha_t()));
    }

    const IECore::LinearToCineonDataConversion<float,boost::uint16_t>& conv_;
};

struct cineon_linear_to_log_lut_fun
{
    cineon_linear_to_log_lut_fun( boost::uint16_t *lut) : lut_( lut) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	using namespace boost::gil;
	float rlin = get_color( src, red_t());
	float glin = get_color( src, green_t());
	float blin = get_color( src, blue_t());

	return image::pixel_t(	lut_[ ((half) rlin).bits()] / 1023.0f,
				lut_[ ((half) glin).bits()] / 1023.0f,
				lut_[ ((half) blin).bits()] / 1023.0f,
				get_color( src, alpha_t()));
    }

    boost::uint16_t *lut_;
};

const float maxAimDensity[3] = { 1.890, 2.046, 2.046};
const float negGamma[3] = {0.49, 0.57, 0.60};

struct exrdpx_log_to_linear_fun
{
    exrdpx_log_to_linear_fun() {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t()) * 1023.0f;
	float g = boost::gil::get_color( src, boost::gil::green_t()) * 1023.0f;
	float b = boost::gil::get_color( src, boost::gil::blue_t()) * 1023.0f;

	r = (r - 445.0f) * (maxAimDensity[0] / 1023.0f);
	g = (g - 445.0f) * (maxAimDensity[1] / 1023.0f);
	b = (b - 445.0f) * (maxAimDensity[2] / 1023.0f);

	r = pow10_h( r / negGamma[0]) * 0.18f;
	g = pow10_h( g / negGamma[1]) * 0.18f;
	b = pow10_h( b / negGamma[2]) * 0.18f;
	return image::pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }
};

struct exrdpx_linear_to_log_fun
{
    exrdpx_linear_to_log_fun() {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t());
	float g = boost::gil::get_color( src, boost::gil::green_t());
	float b = boost::gil::get_color( src, boost::gil::blue_t());

	if( r < HALF_MIN) r = HALF_MIN;
	if( g < HALF_MIN) g = HALF_MIN;
	if( b < HALF_MIN) b = HALF_MIN;

	r = negGamma[0] * log10_h( r / 0.18f);
	g = negGamma[1] * log10_h( g / 0.18f);
	b = negGamma[2] * log10_h( b / 0.18f);

	r = ( r * (1023.0f / maxAimDensity[0]) + 445.0f) / 1023.0f;
	g = ( g * (1023.0f / maxAimDensity[1]) + 445.0f) / 1023.0f;
	b = ( b * (1023.0f / maxAimDensity[2]) + 445.0f) / 1023.0f;

	return image::pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }
};

} // namespace

const char *log_to_linear_node_t::help_string() const
{
    return "Converts between linear and log colorspaces, "
	    "using one of three methods: cineon conversion, "
	    "red log and exrdpx methods";
}

log_to_linear_node_t::log_to_linear_node_t() : pointop_node_t() { set_name("log2lin");}

void log_to_linear_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Convert"));
    p->set_id( "convert");
    p->menu_items() += "Log to Linear", "Linear to Log";
    add_param( p);

    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Method"));
    top->set_id( "method");

    // cineon
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Cineon"));
	group->set_id( "cineon");

	std::auto_ptr<float_param_t> fp( new float_param_t( "Gamma"));
	fp->set_id( "cin_gamma");
	fp->set_range( 0.001, 2);
	fp->set_step( 0.01);
	fp->set_default_value( 0.6);
	group->add_param( fp);

	fp.reset( new float_param_t( "White Point"));
	fp->set_id( "cin_white");
	fp->set_range( 0, 1024);
	fp->set_step( 1);
	fp->set_default_value( 685);
	group->add_param( fp);

	fp.reset( new float_param_t( "Black Point"));
	fp->set_id( "cin_black");
	fp->set_range( 0, 1024);
	fp->set_step( 1);
	fp->set_default_value( 95);
	group->add_param( fp);

	top->add_param( group);
    }

    // redlog
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Red Log"));
	group->set_id( "redlog");
	top->add_param( group);
    }

    // exrdpx
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "ExrDpx"));
	group->set_id( "exrdpx");
	top->add_param( group);
    }

    add_param( top);
}

void log_to_linear_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    if( get_value<int>( param( "convert")))
    {
	switch( get_value<int>( param( "method")))
	{
	case cineon_conv:
	{
	    IECore::LinearToCineonDataConversion<float,boost::uint16_t> conv( get_value<float>( param( "cin_gamma")),
									     get_value<float>( param( "cin_white")),
									     get_value<float>( param( "cin_black")));

	    boost::scoped_array<boost::uint16_t> lut( new boost::uint16_t[1<<16]);
	    init_lut( lut.get(), conv);

	    //cineon_linear_to_log_fun f( conv);
	    cineon_linear_to_log_lut_fun f( lut.get());
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;

	case redlog_conv:
	{
	    IECore::LinearToCineonDataConversion<float,boost::uint16_t> conv( 1.02f, 1023, 0);

	    boost::scoped_array<boost::uint16_t> lut( new boost::uint16_t[1<<16]);
	    init_lut( lut.get(), conv);

	    //cineon_linear_to_log_fun f( conv);
	    cineon_linear_to_log_lut_fun f( lut.get());
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;

	case exrdpx_conv:
	{
	    exrdpx_linear_to_log_fun f;
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;
	}
    }
    else
    {
	switch( get_value<int>( param( "method")))
	{
	case cineon_conv:
	{
	    IECore::CineonToLinearDataConversion<boost::uint16_t, float> conv( get_value<float>( param( "cin_gamma")),
									     get_value<float>( param( "cin_white")),
									     get_value<float>( param( "cin_black")));
	    cineon_log_to_linear_fun f( conv);
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;

	case redlog_conv:
	{
	    IECore::CineonToLinearDataConversion<boost::uint16_t, float> conv( 1.02f, 1023, 0);
	    cineon_log_to_linear_fun f( conv);
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;

	case exrdpx_conv:
	{
	    exrdpx_log_to_linear_fun f;
	    boost::gil::tbb_transform_pixels( src, dst, f);
	}
	break;
	}
    }
}

void log_to_linear_node_t::init_lut( boost::uint16_t *lut,
				    const IECore::LinearToCineonDataConversion<float,boost::uint16_t>& conv) const
{
    for( int i = 0; i < (1<<16); ++i)
    {
	half h;
	h.setBits( i);

	if( h.isNan())
	    lut[i] = 0;
	else
	{
	    if( h.isInfinity())
		lut[i] = h.isNegative() ? 0 : 1023;
	    else
		lut[i] = conv( (float) h);
	}
    }
}

void log_to_linear_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void log_to_linear_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_log_to_linear_node() { return new log_to_linear_node_t();}

node_info_t log_to_linear_node_info()
{
    node_info_t info;
    info.id = "builtin.log2lin";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Log to Linear";
    info.create = &create_log_to_linear_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( log_to_linear_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::log_to_linear_node_t)

