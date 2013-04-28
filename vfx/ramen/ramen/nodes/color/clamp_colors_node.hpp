
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CLAMP_COLORS_NODE_HPP
#define RAMEN_CLAMP_COLORS_NODE_HPP

#include<ramen/nodes/base/pointop_node.hpp>

namespace ramen
{

class clamp_colors_node_t : public pointop_node_t
{
public:

    clamp_colors_node_t();

protected:

    clamp_colors_node_t(const clamp_colors_node_t& other) : pointop_node_t(other) { }
    void operator=(const clamp_colors_node_t&);

private:

    node_t *do_clone() const { return new clamp_colors_node_t(*this);}

    virtual void do_create_params();

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
