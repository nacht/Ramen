// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/gamma_node.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/image/processing.hpp>

namespace ramen
{

gamma_node_t::gamma_node_t() : pointop_node_t()
{
    set_name("gamma");
}

void gamma_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Gamma"));
    p->set_id( "gamma");
    p->set_default_value( 1.0f);
    p->set_range( 0.01f, 5);
    p->set_step( 0.05);
    add_param( p);
}

void gamma_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    image::apply_gamma( src, dst, 1.0f / get_value<float>( param( "gamma")));
}

void gamma_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void gamma_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_gamma_node() { return new gamma_node_t();}

node_info_t gamma_node_info()
{
    node_info_t info;
    info.id = "builtin.gamma";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Gamma";
    info.create = &create_gamma_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( gamma_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::gamma_node_t)
