// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PERCENTILE_FILTER_NODE_HPP
#define	RAMEN_PERCENTILE_FILTER_NODE_HPP

#include<ramen/nodes/base/areaop_node.hpp>

namespace ramen
{

class percentile_filter_node_t : public areaop_node_t
{
public:

    percentile_filter_node_t();

    virtual const char *help_string() const;

protected:

    percentile_filter_node_t( const percentile_filter_node_t& other) : areaop_node_t( other) {}
    void operator=( const percentile_filter_node_t&);

private:

    node_t *do_clone() const { return new percentile_filter_node_t( *this);}

    virtual void do_create_params();

    void do_calc_inputs_expand( const render::render_context_t& context) {}

    virtual void do_process( const render::render_context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif	/* _percentile_filter_NODE_HPP */
