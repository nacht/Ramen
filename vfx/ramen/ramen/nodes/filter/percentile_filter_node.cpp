// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/filter/percentile_filter_node.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/image/percentile_filter.hpp>

namespace ramen
{

const char *percentile_filter_node_t::help_string() const
{
    return "This node can do median filtering, dilating and eroding "
            "depending on the Percent param.";
}

percentile_filter_node_t::percentile_filter_node_t() : areaop_node_t() { set_name( "percent");}

void percentile_filter_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
    p->set_id( "channels");
    p->menu_items() += "RGBA", "RGB", "Alpha";
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Radius"));
    q->set_id( "radius");
    q->set_default_value( 1);
    q->set_range( 0, 50);
    add_param( q);

    q.reset( new float_param_t( "Percent"));
    q->set_id( "percent");
    q->set_static( true);
    q->set_default_value( 0.5);
    q->set_range( 0, 1);
    add_param( q);
}

void percentile_filter_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    hradius = get_value<float>( param( "radius"));
    vradius = hradius;
}

void percentile_filter_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    int radius = get_value<float>( param( "radius")) / context.subsample;

    if( radius == 0)
    {
	boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
	return;
    }

    switch( get_value<int>( param( "channels")))
    {
    case 0:
	image::percentile_filter_rgba( input()->const_subimage_view( area), subimage_view( area), radius, get_value<float>( param( "percent")));
    break;

    case 1:
	image::percentile_filter_rgb( input()->const_subimage_view( area), subimage_view( area), radius, get_value<float>( param( "percent")));
    break;

    case 2:
	image::percentile_filter_alpha( input()->const_subimage_view( area), subimage_view( area), radius, get_value<float>( param( "percent")));
    break;

    default:
	assert( 0);
    }
}

void percentile_filter_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

void percentile_filter_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

// factory
node_t *create_percentile_filter_node() { return new percentile_filter_node_t();}

node_info_t percentile_filter_node_info()
{
    node_info_t info;
    info.id = "builtin.percentile_filter";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Filter";
    info.menu_item = "Percentile Filter";
    info.create = &create_percentile_filter_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( percentile_filter_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::percentile_filter_node_t)
