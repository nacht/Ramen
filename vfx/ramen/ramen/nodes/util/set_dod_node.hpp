// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SET_DOD_NODE_HPP
#define RAMEN_SET_DOD_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class set_dod_node_t : public image_node_t
{
public:

    set_dod_node_t();

    virtual bool use_cache() const { return false;}

protected:

    set_dod_node_t( const set_dod_node_t& other) : image_node_t(other) {}
    void operator=( const set_dod_node_t&);

private:

    node_t *do_clone() const { return new set_dod_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
