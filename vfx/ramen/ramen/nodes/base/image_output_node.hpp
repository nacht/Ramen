// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_OUTPUT_NODE_HPP
#define RAMEN_IMAGE_OUTPUT_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class image_output_node_t : public image_node_t
{
public:

    image_output_node_t();

    virtual bool use_cache() const { return false;}

    virtual void write_image( const render::render_context_t& context) = 0;

protected:

    image_output_node_t( const image_output_node_t& other);
    void operator=( const image_output_node_t&);

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::image_output_node_t)

#endif
