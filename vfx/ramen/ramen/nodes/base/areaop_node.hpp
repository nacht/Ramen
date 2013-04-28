// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_AREAOP_NODE_HPP
#define RAMEN_AREAOP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class areaop_node_t : public image_node_t
{
public:

    areaop_node_t();

protected:

    areaop_node_t( const areaop_node_t& other);
    void operator=( const areaop_node_t&);

    virtual bool expand_defined() const;
    
private:

    virtual void do_calc_bounds( const render::render_context_t& context);

    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const = 0;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    int hradius_, vradius_;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::areaop_node_t)

#endif
