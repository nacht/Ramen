// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/transform/transform_blur_node.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/transform2_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/manipulators/transform2_manipulator.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/image/image_accumulator.hpp>

namespace ramen
{

namespace
{

enum
{
    border_black  = 0,
    border_tile   = 1,
    border_mirror = 2
};

} // unnamed

transform_blur_node_t::transform_blur_node_t() : image_node_t()
{
    set_name("move2_blur");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void transform_blur_node_t::do_create_params()
{
    std::auto_ptr<transform2_param_t> xp( new transform2_param_t( "xform", "xf"));
    add_param( xp);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    std::auto_ptr<float_param_t> p( new float_param_t( "Samples"));
    p->set_id( "samples");
    p->set_min( 1);
    p->set_default_value( 1);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> q( new popup_param_t( "Borders"));
    q->set_id( "borders");
    q->menu_items() += "Black", "Tile", "Mirror";
    add_param( q);
}

void transform_blur_node_t::do_create_manipulators()
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    assert( p);

    std::auto_ptr<transform2_manipulator_t> m( new transform2_manipulator_t( p));
    add_manipulator( m);
}

void transform_blur_node_t::do_calc_domain( const render::render_context_t& context)   { set_domain( input()->domain());}

void transform_blur_node_t::do_calc_bounds( const render::render_context_t& context)
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    assert( p);

    int num_samples = (get_value<int>( param( "samples")) * 2) + 1;

    Imath::Box2i area;

    for( int i = 0; i < num_samples; ++i)
    {
        boost::optional<Imath::M33d> m( p->xform_blur_matrix_at_time( context.time, (float) i / ( num_samples - 1) , context.subsample));

        if( m)
        {
            Imath::Box2i bounds( transform( input()->bounds(), m.get(), false));
            area.extendBy( bounds);
        }
    }

    set_bounds( area);
}

void transform_blur_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    if( interest().isEmpty())
        return;

    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    assert( p);

    int num_samples = (get_value<int>( param( "samples")) * 2) + 1;

    Imath::Box2i roi;

    for( int i = 0; i < num_samples; ++i)
    {
        boost::optional<Imath::M33d> m( p->xform_blur_matrix_at_time( context.time, (float) i / ( num_samples - 1), context.subsample));

        if( m)
        {
            try
            {
                Imath::M33d inv_m( m.get().inverse( true));
                Imath::Box2i box( transform( interest(), inv_m, false));
                roi.extendBy( box);
            }
            catch( Iex::MathExc& e)
            {
            }
        }
    }

    if( !roi.isEmpty())
    {
        // add some margin for filtering
        roi.min.x -= 2 * context.subsample;
        roi.min.y -= 2 * context.subsample;
        roi.max.x += 2 * context.subsample;
        roi.max.y += 2 * context.subsample;
        input()->add_interest( roi);
    }
}

void transform_blur_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def( intersect( bounds(), interest()));
    set_defined( def);
}

void transform_blur_node_t::do_process( const render::render_context_t& context)
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    assert( p);

    int num_samples = ( get_value<int>( param( "samples")) * 2) + 1;
    int border_mode = get_value<int>( param( "borders"));

    image::image_accumulator_t acc( image_view());

    image::image_t tmp( image_view().width(), image_view().height());

    float sumw = 0.0f;

    for( int i = 0; i < num_samples; ++i)
    {
        boost::optional<Imath::M33d> m( p->xform_blur_matrix_at_time( context.time, (float) i / ( num_samples - 1), context.subsample));

        boost::gil::fill_pixels( boost::gil::view( tmp), image::pixel_t( 0, 0, 0, 0));

        do_process( boost::gil::view( tmp), m, border_mode);

        float w = 1.0f;
        acc.accumulate( boost::gil::view( tmp), w);

        sumw += w;
    }

    if( sumw != 0.0f)
        acc.multiply( 1.0f / sumw);
}

void transform_blur_node_t::do_process( const image::image_view_t& dst, const boost::optional<Imath::M33d>& xf, int border_mode)
{
    if( input()->defined().isEmpty() || !xf)
        return;

    try
    {
        Imath::M33d inv_xf = xf.get().inverse( true);

        if( border_mode == border_black)
            image::affine_warp_bilinear( input()->defined(), input()->const_image_view(), defined(), dst, xf.get(), inv_xf);
        else
        {
            if( border_mode == border_tile)
                image::affine_warp_bilinear_tile( input()->defined(), input()->const_image_view(), defined(), dst, xf.get(), inv_xf);
            else
                image::affine_warp_bilinear_mirror( input()->defined(), input()->const_image_view(), defined(), dst, xf.get(), inv_xf);
        }
    }
    catch( Iex::MathExc)
    {
    }
}

void transform_blur_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void transform_blur_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

// factory

node_t *create_transform_blur_node() { return new transform_blur_node_t();}

node_info_t transform_blur_node_info()
{
    node_info_t info;
    info.id = "builtin.transform_blur";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Transform Blur";
    info.create = &create_transform_blur_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( transform_blur_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::transform_blur_node_t)
