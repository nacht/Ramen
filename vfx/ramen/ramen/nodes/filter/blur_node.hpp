// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BLUR_NODE_HPP
#define	RAMEN_BLUR_NODE_HPP

#include<ramen/nodes/base/areaop_node.hpp>

namespace ramen
{

class blur_node_t : public areaop_node_t
{
public:

    blur_node_t();

protected:

    blur_node_t( const blur_node_t& other) : areaop_node_t( other) {}
    void operator=( const blur_node_t&);

    virtual bool expand_defined() const;

private:

    node_t *do_clone() const { return new blur_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::render_context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif

