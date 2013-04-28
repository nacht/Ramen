// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_AVERAGE_NODE_HPP
#define RAMEN_AVERAGE_NODE_HPP

#include<ramen/nodes/base/timeop_node.hpp>

namespace ramen
{

class average_node_t : public timeop_node_t
{
public:

    average_node_t();

protected:

    average_node_t(const average_node_t& other) : timeop_node_t(other) { }
    void operator=(const average_node_t&);

private:

    node_t *do_clone() const { return new average_node_t(*this);}

    virtual void do_create_params();

    void get_frame_range( float time, int& start, int& end) const;
    
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_calc_hash( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
