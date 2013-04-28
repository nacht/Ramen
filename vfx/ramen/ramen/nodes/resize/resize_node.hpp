// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RESIZE_NODE_HPP
#define RAMEN_RESIZE_NODE_HPP

#include<ramen/nodes/base/rescale_node.hpp>

namespace ramen
{

class resize_node_t : public rescale_node_t
{
public:

    resize_node_t();

protected:

    resize_node_t(const resize_node_t& other) : rescale_node_t(other) {}
    void operator=(const resize_node_t&);

private:

    node_t *do_clone() const { return new resize_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
