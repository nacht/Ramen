// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ADD_MIX_ADD_MIX_LAYER_NODE_HPP
#define RAMEN_ADD_MIX_ADD_MIX_LAYER_NODE_HPP

#include<ramen/nodes/base/base_layer_node.hpp>

namespace ramen
{

class add_mix_layer_node_t : public base_layer_node_t
{
public:

    add_mix_layer_node_t();

protected:

    add_mix_layer_node_t(const add_mix_layer_node_t& other) : base_layer_node_t(other) { }
    void operator=(const add_mix_layer_node_t&);

private:

    node_t *do_clone() const { return new add_mix_layer_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_domain( const render::render_context_t& context);
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
