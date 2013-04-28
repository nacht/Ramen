// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_MOVE2D_NODE_HPP
#define RAMEN_IMAGE_MOVE2D_NODE_HPP

#include<ramen/nodes/base/image_xform_node.hpp>

namespace ramen
{

class move2d_node_t : public image_xform_node_t
{
public:

    move2d_node_t();

protected:

    move2d_node_t(const move2d_node_t& other) : image_xform_node_t( other) {}
    void operator=(const move2d_node_t&);

private:

    node_t *do_clone() const { return new move2d_node_t(*this);}

    virtual void do_create_params();
    virtual void do_create_manipulators();

    virtual boost::optional<Imath::M33d> do_calc_transform_matrix_at_time( float time, int subsample = 1) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
