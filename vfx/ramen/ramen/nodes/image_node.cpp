// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

image_node_t::image_node_t() : node_t() {}

image_node_t::image_node_t( const image_node_t& other) : node_t( other) {}

void image_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( node_t);
}

void image_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::image_node_t)
