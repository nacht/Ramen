// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FULL_FRAME_NODE_HPP
#define RAMEN_FULL_FRAME_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class full_frame_node_t : public image_node_t
{
public:

    full_frame_node_t();

protected:

    full_frame_node_t( const full_frame_node_t& other);
    void operator=( const full_frame_node_t&);

private:

    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::full_frame_node_t)

#endif
