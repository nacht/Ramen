// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RESCALE_NODE_HPP
#define RAMEN_RESCALE_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class rescale_node_t : public image_node_t
{
public:

    rescale_node_t();

protected:

    rescale_node_t(const rescale_node_t& other) : image_node_t( other) {}
    void operator=(const rescale_node_t&);

    virtual void do_create_params();

    virtual void do_calc_domain( const render::render_context_t& context);
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);
    
    Imath::Box2i needed_area() const;

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

protected:

    float scale_x_, scale_y_;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::rescale_node_t)

#endif
