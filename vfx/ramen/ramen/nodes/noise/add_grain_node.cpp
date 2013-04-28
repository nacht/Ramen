// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/noise/add_grain_node.hpp>

#include<cmath>

#include<boost/lexical_cast.hpp>

#include<tbb/parallel_for.h>

#include<OpenEXR/ImathFun.h>
#include<OpenEXR/ImathRandom.h>

#include<noise/simplex_noise_generator.hpp>

#include<ramen/params/group_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float3_param.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/color.hpp>

namespace ramen
{

namespace
{

struct apply_grain_fun
{
    apply_grain_fun( image::const_image_view_t& src_view, image::image_view_t& dst_view,
                      const Imath::Box2i& area, const Imath::V3f& freq, float aspect, const Imath::V3f intensity,
                      const Imath::V3f channel_offset, float lo, float mid, float hi, float soft, float sat) : src_view_( src_view), dst_view_( dst_view)
    {
        area_ = area;
        freq_ = freq;
        aspect_ = aspect;
        intensity_ = intensity;

        channel_offset_ = channel_offset;

        lo_intensity_ = lo;
        mid_intensity_= mid;
        hi_intensity_ = hi;

        softness_ = std::min( 1.0f - soft, 0.95f);

        saturation_ = sat;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        for( int j = r.begin(); j != r.end(); ++j)
        {
            image::const_image_view_t::x_iterator   src_it( src_view_.row_begin( j - area_.min.y));
            image::image_view_t::x_iterator         dst_it( dst_view_.row_begin( j - area_.min.y));

            for( int i = area_.min.x; i <= area_.max.x; ++i)
            {
                float nx = noise::simplex_noise_generator_t::noise( freq_.x * i * aspect_, freq_.x * j, channel_offset_.x);
                float ny = noise::simplex_noise_generator_t::noise( freq_.y * i * aspect_, freq_.y * j, channel_offset_.y);
                float nz = noise::simplex_noise_generator_t::noise( freq_.z * i * aspect_, freq_.z * j, channel_offset_.z);

                // contrast
                nx = softness( nx);
                ny = softness( ny);
                nz = softness( nz);

                // saturation
                if( saturation_ != 1.0f)
                {
                    float nl = nx * 0.212671f + ny * 0.715160f + nz * 0.072169f;
                    nx = Imath::lerp( nl, nx, saturation_);
                    ny = Imath::lerp( nl, ny, saturation_);
                    nz = Imath::lerp( nl, nz, saturation_);
                }

                Imath::V3f amount = intensity_;

                // levels
                float y = image::luminance( *src_it);
                float lo_w  = image::shadow_blend_function( y);
                float hi_w  = image::highlights_blend_function( y);
                float mid_w = 1.0f - hi_w - lo_w;
                amount *= ( lo_intensity_ * lo_w) + ( mid_intensity_ * mid_w) + ( hi_intensity_ * hi_w);

                // apply
                float rr = boost::gil::get_color( *src_it, boost::gil::red_t())   + ( amount.x * nx);
                float rg = boost::gil::get_color( *src_it, boost::gil::green_t()) + ( amount.y * ny);
                float rb = boost::gil::get_color( *src_it, boost::gil::blue_t())  + ( amount.z * nz);

                // clamp negative values and write
                *dst_it = image::pixel_t( std::max( rr, 0.0f), std::max( rg, 0.0f), std::max( rb, 0.0f), boost::gil::get_color( *src_it, boost::gil::alpha_t()));

                ++src_it;
                ++dst_it;
            }
        }
    }

private:

    float softness( float x) const
    {
        float xx = x;

        if( x < 0)
            xx = -xx;

	if( xx <= softness_)
	    return 0.0f;
	else
            xx = ( xx - softness_) / ( 1.0f - softness_);

        if( x < 0)
            xx = -xx;

        return xx;
    }

    image::const_image_view_t& src_view_;
    image::image_view_t& dst_view_;
    Imath::Box2i area_;

    Imath::V3f channel_offset_;
    Imath::V3f freq_, intensity_;
    float aspect_;

    float lo_intensity_, mid_intensity_, hi_intensity_;

    float softness_;
    float saturation_;
};

} // unnamed


add_grain_node_t::add_grain_node_t() : pointop_node_t()
{ 
    set_name( "add_grain");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["matte plug"], "Mask"), true);
}

void add_grain_node_t::do_create_params()
{
    std::auto_ptr<group_param_t> top( new group_param_t( "General"));
    {
        std::auto_ptr<float_param_t> p( new float_param_t( "Intensity"));
        p->set_id( "intensity");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Grain Freq"));
        p->set_id( "grain_freq");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Aspect"));
        p->set_id( "grain_aspect");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Softness"));
        p->set_id( "soft");
        p->set_range( 0, 1);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Saturation"));
        p->set_id( "saturation");
        p->set_range( 0, 1);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Seed"));
        p->set_id( "seed");
        p->set_default_value( .7135);
        p->set_step( 0.05f);
        top->add_param( p);
    }
    add_param( top);

    top.reset( new group_param_t( "Channels"));
    {
        std::auto_ptr<float3_param_t> p( new float3_param_t( "Intensity"));
        p->set_id( "channel_intensity");
        p->set_min( 0);
        p->set_default_value( Imath::V3f( 1, 1, 1));
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float3_param_t( "Grain Freq"));
        p->set_id( "channel_grain_freq");
        p->set_min( 0);
        p->set_default_value( Imath::V3f( 1, 1, 1));
        p->set_step( 0.05f);
        top->add_param( p);
    }
    add_param( top);

    top.reset( new group_param_t( "Levels"));
    {
        std::auto_ptr<float_param_t> p( new float_param_t( "Shadows"));
        p->set_id( "shd_intensity");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Midtones"));
        p->set_id( "mid_intensity");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);

        p.reset( new float_param_t( "Highlights"));
        p->set_id( "hi_intensity");
        p->set_min( 0);
        p->set_default_value( 1);
        p->set_step( 0.05f);
        top->add_param( p);
    }
    add_param( top);
}

void add_grain_node_t::do_calc_hash( const render::render_context_t& context)
{
    std::string t = boost::lexical_cast<std::string>( std::floor( (double) context.time));
    append_to_hash( t);
}

void add_grain_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    image::const_image_view_t src_view;
    image::image_view_t dst_view;
    Imath::Box2i area;

    if( input( 1))
    {
	boost::gil::copy_pixels( src, dst);
	area = intersect( input( 1)->defined(), defined());

	if( area.isEmpty())
	    return;

	src_view = input( 0)->const_subimage_view( area);
	dst_view = subimage_view( area);
    }
    else
    {
	src_view = src;
	dst_view = dst;
        area = defined();
    }

    // Init the rng, this is a first approx.
    // a random hash is the way to go (I think).
    float seed = get_value<float>( param( "seed"));
    const char *p = reinterpret_cast<const char *>( &seed);
    Imath::Rand48 rng( p[0] | ( p[1] << 8) | ( p[2] << 16) | ( p[3] << 24));

    // advance the rng
    if( context.time > 0)
    {
        for( int i = 0; i < context.time; ++i)
        {
            rng.nextf(); rng.nextf();
            rng.nextf();
        }
    }
    else
    {
        for( int i = 0; i > context.time; --i)
        {
            rng.nextf(); rng.nextf();
            rng.nextf();
        }
    }

    Imath::V3f channel_offset( rng.nextf( 0, 10), rng.nextf( 0, 10), rng.nextf( 0, 10));

    Imath::V3f intensity( get_value<Imath::V3f>( param( "channel_intensity")));
    intensity *= get_value<float>( param( "intensity")) * 0.1f;

    Imath::V3f grain_size(  get_value<Imath::V3f>( param( "channel_grain_freq")));
    grain_size *= get_value<float>( param( "grain_freq")) * 0.1f * context.subsample;

    float aspect = get_value<float>( param( "grain_aspect"));

    apply_grain_fun f( src_view, dst_view, area, grain_size, aspect, intensity, channel_offset,
                        get_value<float>( param( "shd_intensity")),
                        get_value<float>( param( "mid_intensity")),
                        get_value<float>( param( "hi_intensity")),
                        get_value<float>( param( "soft")),
                        get_value<float>( param( "saturation")));

    tbb::parallel_for( tbb::blocked_range<int>( area.min.y, area.max.y + 1), f, tbb::auto_partitioner());

    if( input( 1))
        image::key_mix( src_view, dst_view, boost::gil::nth_channel_view( input(1)->const_subimage_view( area), 3), dst_view);
}

void add_grain_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void add_grain_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_add_grain_node() { return new add_grain_node_t();}

node_info_t add_grain_node_info()
{
    node_info_t info;
    info.id = "builtin.add_grain";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Noise";
    info.menu_item = "Add Grain";
    info.create = &create_add_grain_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( add_grain_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::add_grain_node_t)
