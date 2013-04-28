// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UNSHARP_MASK_NODE_HPP
#define	RAMEN_UNSHARP_MASK_NODE_HPP

#include<ramen/nodes/base/areaop_node.hpp>

namespace ramen
{

class unsharp_mask_node_t : public areaop_node_t
{
public:

    unsharp_mask_node_t();

protected:

    unsharp_mask_node_t( const unsharp_mask_node_t& other) : areaop_node_t( other) {}
    void operator=( const unsharp_mask_node_t&);

private:

    node_t *do_clone() const { return new unsharp_mask_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::render_context_t& context);
    
    virtual void get_expand_radius( int& hradius, int& vradius) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
