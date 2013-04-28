// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/time/average_node.hpp>

#include<boost/bind.hpp>

#include<ramen/dataflow/algorithm.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/render/renderer.hpp>

#include<ramen/image/image_accumulator.hpp>

#include "ramen/app/composition.hpp"


namespace ramen
{

average_node_t::average_node_t() : timeop_node_t() { set_name("average");}

void average_node_t::do_create_params()
{
    {
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Method"));
    p->set_id( "method");
    p->menu_items() += "Past & Future", "Past", "Future";
    add_param( p);
    }

    {
    std::auto_ptr<float_param_t> p( new float_param_t( "Frames"));
    p->set_id( "frames");
    p->set_range( 1, 100);
    p->set_default_value( 2);
    add_param( p);
    }
}

void average_node_t::get_frame_range( float time, int& start, int& end) const
{
    int method = get_value<int>( param( "method"));
    int num_frames = get_value<int>( param( "frames"));

    if( method == 0)
    {
	--num_frames;
	int extra = 0;

	if( num_frames & 1)
	 extra = 1;

	start = time - ( (num_frames - extra) / 2);
	end   = time + ( num_frames / 2) + extra;
    }
    else
    {
	if( method == 1)
	{
	    start = time - num_frames + 1;
	    end   = time;
	}
	else
	{
	    start = time;
	    end   = time + num_frames -1;
	}
    }
}

void average_node_t::do_calc_bounds( const render::render_context_t& context)
{
    Imath::Box2i box;
    int t0, t1;
    get_frame_range( context.time, t0, t1);

    render::render_context_t new_context = context;
    new_context.result_node = input();

    // we can't use the renderer here to compute the areas at each time,
    // as it would enter infinite recursion when the render state is
    // restored, so we do it manually.

    for( int i = t0; i <= t1; ++i)
    {
	new_context.time = i;
        composition()->set_frame( i);
        dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_domain, _1, new_context));
        dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_bounds, _1, new_context));
        box.extendBy( input()->bounds());
    }

    // restore the time & areas
    composition()->set_frame( context.time);
    dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_domain, _1, context));
    dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_bounds, _1, context));

    set_bounds( box);
}

void average_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( intersect( bounds(), interest()));
}

void average_node_t::do_calc_hash( const render::render_context_t& context)
{
    if( !is_valid())
        return;

    clear_hash();
    hash_generator() << typeid( *this).name();

    int t0, t1;
    get_frame_range( context.time, t0, t1);

    render::render_context_t new_context = context;
    new_context.result_node = input();

    for( int i = t0; i <= t1; ++i)
    {
	new_context.time = i;
        composition()->set_frame( i);
        dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::clear_hash, _1));
        dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_hash, _1, new_context));
        hash_generator() << input()->hash_str();
    }

    // restore the time & hashes
    composition()->set_frame( context.time);
    dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::clear_hash, _1));
    dataflow::depth_first_inputs_search( *input(), boost::bind( &node_t::calc_hash, _1, context));

    hash_generator() << param_set().str();
}

void average_node_t::do_process( const render::render_context_t& context)
{
    if( defined().isEmpty())
	return;

    Imath::Box2i area( intersect( input()->defined(), defined()));
    boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
    input()->release_image();

    int t0, t1;
    get_frame_range( context.time, t0, t1);

    image::image_accumulator_t accum( image_view(), defined());
    render::render_context_t new_context = context;
    new_context.result_node = input();
    new_context.interest = interest();

    for( int i = t0; i <= t1; ++i)
    {
	if( i != context.time)
	{
	    new_context.time = i;
	    render::renderer_t renderer( new_context, true);
	    renderer.render( interest());
	    accum.accumulate( input()->const_image_view(), input()->defined());
            input()->release_image();
	}
    }

    accum.multiply( 1.0f / ( t1 - t0 + 1));
}

void average_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( timeop_node_t);
}

void average_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( timeop_node_t);
}

// factory
node_t *create_average_node() { return new average_node_t();}

node_info_t average_node_info()
{
    node_info_t info;
    info.id = "builtin.average";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Time";
    info.menu_item = "Average";
    info.create = &create_average_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( average_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::average_node_t)
