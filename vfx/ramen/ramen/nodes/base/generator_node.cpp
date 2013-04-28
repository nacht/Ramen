// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/generator_node.hpp>

#include<ramen/params/format_param.hpp>

namespace ramen
{

generator_node_t::generator_node_t() : image_node_t()
{
    add_output_plug();
}

generator_node_t::generator_node_t( const generator_node_t& other) : image_node_t( other) {}

void generator_node_t::do_create_params()
{
    std::auto_ptr<format_param_t> p( new format_param_t( "Format"));
    p->set_id( "format");
    add_param( p);
}

void generator_node_t::do_calc_domain( const render::render_context_t& context)
{
    Imath::Box2i area( get_value<format_t>( param( "format")).area());
    --area.max.x;
    --area.max.y;
    set_domain( area);
}

void generator_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void generator_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::generator_node_t)
