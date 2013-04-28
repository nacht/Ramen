// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SELECT_NODE_HPP
#define	RAMEN_SELECT_NODE_HPP

#include<ramen/nodes/base/multi_input_node.hpp>

namespace ramen
{

class select_node_t : public multi_input_node_t
{
public:

    select_node_t();

    virtual bool use_cache() const { return false;}

protected:

    select_node_t( const select_node_t& other) : multi_input_node_t( other) {}
    void operator=( const select_node_t&);

private:

    virtual node_t *do_clone() const { return new select_node_t( *this);}

    virtual void do_create_params();

    virtual void do_connected( node_t *src, int port);

    virtual bool do_is_valid() const;

    virtual bool do_include_input_in_hash( int i) const;

    virtual void do_calc_domain( const render::render_context_t& context);
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_recursive_process( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif	/* _select_NODE_HPP */
