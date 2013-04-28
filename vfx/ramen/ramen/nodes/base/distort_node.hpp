// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#ifndef RAMEN_DISTORT_NODE_HPP
#define RAMEN_DISTORT_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class distort_node_t : public image_node_t
{
public:

    distort_node_t();

protected:

    distort_node_t( const distort_node_t& other);
    void operator=( const distort_node_t&);

    bool needs_full_input_frame() const { return false;}
    virtual void get_expand_radius( int& hradius, int& vradius) const;

private:

    void do_calc_inputs_interest( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    int hradius_, vradius_;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::distort_node_t)

#endif
