// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/resize/resize_node.hpp>

#include<gil/extension/rescale/tbb_rescale.hpp>
#include<gil/extension/rescale/filters.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/params/popup_param.hpp>

namespace ramen
{

namespace
{

enum
{
    mitchell_filter = 0,
    catmull_filter,
    lanczos_filter
};

} // unnamed

resize_node_t::resize_node_t() : rescale_node_t() { set_name( "resize");}

void resize_node_t::do_create_params()
{
    rescale_node_t::do_create_params();

    std::auto_ptr<popup_param_t> p( new popup_param_t( "Filter"));
    p->set_id( "filter");
    p->menu_items() += "Mitchell", "Catmull", "Lanczos";
    add_param( p);
}

void resize_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i src_area( needed_area());
    Imath::Box2i dst_area( scale( input()->defined(), scale_x_, scale_y_, input()->domain().min.x, input()->domain().min.y));

    dst_area = intersect( dst_area, defined());

    if( src_area.isEmpty() || dst_area.isEmpty())
	return;

    // avoid filtering in this case
    if( scale_x_ == 1.0f && scale_y_ == 1.0f)
    {
	boost::gil::copy_pixels( input()->const_subimage_view( src_area), subimage_view( dst_area));
	return;
    }

    switch( get_value<int>( param( "filter")))
    {
    case mitchell_filter:
	boost::gil::tbb_rescale( input()->const_subimage_view( src_area), subimage_view( dst_area), boost::gil::bicubic_filter());
    break;

    case catmull_filter:
	boost::gil::tbb_rescale( input()->const_subimage_view( src_area), subimage_view( dst_area), boost::gil::catmull_rom_filter());
    break;

    case lanczos_filter:
	boost::gil::tbb_rescale( input()->const_subimage_view( src_area), subimage_view( dst_area), boost::gil::lanczos_filter());
    break;
    }
}

void resize_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( rescale_node_t);
}

void resize_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( rescale_node_t);
}

// factory
node_t *create_resize_node() { return new resize_node_t();}

node_info_t resize_node_info()
{
    node_info_t info;
    info.id = "builtin.resize";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Resize";
    info.create = &create_resize_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( resize_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::resize_node_t)
