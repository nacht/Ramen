// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/base_layer_node.hpp>

namespace ramen
{

base_layer_node_t::base_layer_node_t() : image_node_t() {}

base_layer_node_t::base_layer_node_t( const base_layer_node_t& other) : image_node_t( other) {}

void base_layer_node_t::do_recursive_process( const render::render_context_t& context)
{
    // for layer nodes, we can render and composite
    // one layer at a time. So we render the inputs on
    // demand in the process method of each derived class.
    // It's a bit more work, but it's also more efficient
    // and we can also avoid rendering a layer if it's not
    // visible i.e. opacity == 0
    alloc_image();
    process( context);
}

void base_layer_node_t::render_input( std::size_t i, const render::render_context_t& context)
{
    assert( input( i) && "Base layer node, null input node in render input");
    input( i)->recursive_process( context);
}

void base_layer_node_t::release_input_image( std::size_t i)
{ 
    assert( input( i) && "Base layer node, null input node in release input image");
    input( i)->release_image();
}

void base_layer_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void base_layer_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::base_layer_node_t)
