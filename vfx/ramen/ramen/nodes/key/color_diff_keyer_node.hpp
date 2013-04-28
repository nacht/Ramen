// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLORDIFF_KEYER_NODE_HPP
#define RAMEN_COLORDIFF_KEYER_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class color_diff_keyer_node_t : public image_node_t
{
public:

    color_diff_keyer_node_t();

    virtual bool use_cache() const { return false;}

protected:

    color_diff_keyer_node_t(const color_diff_keyer_node_t& other) : image_node_t(other) { }
    void operator=(const color_diff_keyer_node_t&);

private:

    node_t *do_clone() const { return new color_diff_keyer_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
