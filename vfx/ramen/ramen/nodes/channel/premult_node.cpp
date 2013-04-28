// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/channel/premult_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

const char *premult_node_t::help_string() const
{
    return "Multiplies or divides the RGB channels by the alpha channel.";
}

premult_node_t::premult_node_t() : pointop_node_t() { set_name("premult");}

void premult_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->add_menu_item( "Premult");
    p->add_menu_item( "Unpremult");
    add_param( std::auto_ptr<param_t>( p.release()));
}

void premult_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    int div = get_value<int>( param( "mode"));

    if( div)
	image::unpremultiply( src, dst);
    else
	image::premultiply( src, dst);
}

void premult_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void premult_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_premult_node() { return new premult_node_t();}

node_info_t premult_node_info()
{
    node_info_t info;
    info.id = "builtin.premult";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Channel";
    info.menu_item = "Alpha Premult";
    info.create = &create_premult_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( premult_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::premult_node_t)
