// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/image_xform_node.hpp>

#include<vector>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/image/processing.hpp>
#include<ramen/image/image_accumulator.hpp>

namespace ramen
{

image_xform_node_t::image_xform_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
    set_can_motion_blur( true);
}

image_xform_node_t::filter_type image_xform_node_t::get_filter_type() const
{
    return (filter_type) get_value<int>( param( "filter"));
}

const motion_blur_param_t *image_xform_node_t::motion_blur_param() const
{
    // QUESTION: is it ok to assume all xform nodes have a motion blur param?
    const param_t *p = &param( "motion_blur");
    const motion_blur_param_t *mb = dynamic_cast<const motion_blur_param_t*>( p);
    assert( mb);
    return mb;
}

image_node_t *image_xform_node_t::get_img_source()
{
    image_node_t *source = input_as<image_node_t>();

    while( 1)
    {
	image_xform_node_t *x = dynamic_cast<image_xform_node_t*>( source);

	if( !x)
	    break;

	if( !source->input_as<image_node_t>())
	    break;

	source = source->input_as<image_node_t>();
    }

    return source;
}

const image_node_t *image_xform_node_t::get_img_source_and_xform_list( std::vector<const image_xform_node_t*>& xforms) const
{
    xforms.push_back( this);

    const image_node_t *source = input_as<image_node_t>();

    while( 1)
    {
	const image_xform_node_t *x = dynamic_cast<const image_xform_node_t*>( source);

	if( !x)
	    break;

	xforms.push_back( x);

	if( !source->input_as<image_node_t>())
	    break;

	source = source->input_as<image_node_t>();
    }

    return source;
}

boost::optional<Imath::M33d> image_xform_node_t::calc_transform_matrix_at_time( float time, int subsample) const
{
    if( ignored())
    {
        Imath::M33d m;
        m.makeIdentity();
        return boost::optional<Imath::M33d>( m);
    }

    return do_calc_transform_matrix_at_time( time, subsample);
}

boost::optional<Imath::M33d> image_xform_node_t::global_matrix_at_time( float time, const std::vector<const image_xform_node_t*>& xforms, int subsample) const
{
    Imath::M33d global;
    global.makeIdentity();

    for( int i = xforms.size() - 1; i >= 0; --i)
    {
        boost::optional<Imath::M33d> local( xforms[i]->calc_transform_matrix_at_time( time, subsample));

        if( !local)
            return boost::optional<Imath::M33d>();

        global *= local.get();
    }

    return boost::optional<Imath::M33d>( global);
}

Imath::Box2i image_xform_node_t::projective_transform_box( const Imath::Box2i& box, const Imath::M33d& m, int step, bool round_up) const
{
    int ystep = box.size().y / step;
    int xstep = box.size().x / step;

    Imath::Box2d result;
    Imath::V2d p, q;

    for( int j = box.min.y; j < box.max.y; j += ystep)
    {
	for( int i = box.min.x; i < box.max.x; i += xstep)
	{
	    p = Imath::V2d( i, j);
	    q = p * m;
            result.extendBy( q);
	}
    }

    // add the remaining corners
    p = Imath::V2d( box.max.x, box.min.y);
    q = p * m;
    result.extendBy( q);

    p = Imath::V2d( box.max.x, box.max.y);
    q = p * m;
    result.extendBy( q);

    p = Imath::V2d( box.min.x, box.max.y);
    q = p * m;
    result.extendBy( q);

    if( round_up)
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<double>::floor( result.min.x), Imath::Math<double>::floor( result.min.y)),
                             Imath::V2i( Imath::Math<double>::ceil( result.max.x) , Imath::Math<double>::ceil( result.max.y)));
    }
    else
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<double>::ceil( result.min.x) , Imath::Math<double>::ceil( result.min.y)),
                             Imath::V2i( Imath::Math<double>::floor( result.max.x), Imath::Math<double>::floor( result.max.y)));
    }
}

void image_xform_node_t::do_calc_domain( const render::render_context_t& context)   { set_domain( input()->domain());}

void image_xform_node_t::do_calc_bounds( const render::render_context_t& context)
{
    std::vector<const image_xform_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list( xforms);

    if( !src)
        return;

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples,
                                                                            context.motion_blur_shutter_factor));

    float t = d.start_time;
    Imath::Box2i area;

    for( int i = 0; i < d.num_samples; ++i)
    {
        boost::optional<Imath::M33d> m( global_matrix_at_time( t, xforms));

        if( m)
        {
            //Imath::Box2i bounds( transform( src->bounds(), m.get(), false));
            Imath::Box2i bounds( projective_transform_box( src->bounds(), m.get(), 50));
            area.extendBy( bounds);
        }

        t += d.time_step;
    }

    set_bounds( area);
}

void image_xform_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    if( interest().isEmpty())
        return;

    std::vector<const image_xform_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list( xforms);

    if( !src)
        return;

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples,
                                                                            context.motion_blur_shutter_factor));

    float t = d.start_time;
    Imath::Box2i roi;

    for( int i = 0; i < d.num_samples; ++i)
    {
        boost::optional<Imath::M33d> m( global_matrix_at_time( t, xforms));

        if( m)
        {
            try
            {
                Imath::M33d inv_m( m.get().inverse( true));
                Imath::Box2i box( projective_transform_box( interest(), inv_m, 50));
                //Imath::Box2i box( transform( interest(), inv_m, false));
                roi.extendBy( box);
            }
            catch( Iex::MathExc& e)
            {
            }
        }

        t += d.time_step;
    }

    if( !roi.isEmpty())
    {
        // add some margin for filtering
        roi.min.x -= 2 * context.subsample;
        roi.min.y -= 2 * context.subsample;
        roi.max.x += 2 * context.subsample;
        roi.max.y += 2 * context.subsample;

        // fix this, it's ugly..............
        const_cast<image_node_t*>( src)->add_interest( roi);
    }
}

void image_xform_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def( intersect( bounds(), interest()));
    set_defined( def);
}

void image_xform_node_t::do_recursive_process( const render::render_context_t& context)
{
    if( defined().isEmpty())
        return;

    image_node_t *src = get_img_source();

    if( src)
    {
        src->recursive_process( context);
        alloc_image();
        process( context);
        src->release_image();
    }
}

void image_xform_node_t::do_process( const render::render_context_t& context)
{
    std::vector<const image_xform_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list( xforms);

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples, context.motion_blur_shutter_factor));

    if( d.num_samples == 1)
    {
        boost::optional<Imath::M33d> xf( global_matrix_at_time( context.time, xforms, context.subsample));
        do_process( src, image_view(), xf);
    }
    else
    {
        image::image_accumulator_t acc( image_view());

        image::image_t tmp( image_view().width(), image_view().height());

        float t = d.start_time;
        float sumw = 0.0f;

        for( int i = 0; i < d.num_samples; ++i)
        {
            boost::optional<Imath::M33d> xf( global_matrix_at_time( t, xforms, context.subsample));

            boost::gil::fill_pixels( boost::gil::view( tmp), image::pixel_t( 0, 0, 0, 0));
            do_process( src, boost::gil::view( tmp), xf);

            float w = d.weight_for_time( t);
            acc.accumulate( boost::gil::view( tmp), w);

            t += d.time_step;
            sumw += w;
        }

        if( sumw != 0.0f)
            acc.multiply( 1.0f / sumw);
    }
}

void image_xform_node_t::do_process( const image_node_t *src, const image::image_view_t& dst, const boost::optional<Imath::M33d>& xf)
{
    if( src->defined().isEmpty() || !xf)
        return;

    try
    {
        Imath::M33d inv_xf = xf.get().inverse( true);

        if( isAffine( xf.get()))
        {
            switch( get_filter_type())
            {
            case filter_point:
                image::affine_warp_nearest( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;

            case filter_bilinear:
                image::affine_warp_bilinear( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;

            case filter_bicubic:
                image::affine_warp_bicubic( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;
            }
        }
        else
        {
            switch( get_filter_type())
            {
            case filter_point:
                image::projective_warp_nearest( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;

            case filter_bilinear:
                image::projective_warp_bilinear( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;

            case filter_bicubic:
                image::projective_warp_bicubic( src->defined(), src->const_image_view(), defined(), dst, xf.get(), inv_xf);
            break;
            }
        }
    }
    catch( Iex::MathExc)
    {
    }
}

void image_xform_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

void image_xform_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::image_xform_node_t)
