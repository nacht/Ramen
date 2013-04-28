// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORNER_PIN_NODE_HPP
#define RAMEN_CORNER_PIN_NODE_HPP

#include<ramen/nodes/base/image_xform_node.hpp>

#include<boost/array.hpp>
#include<boost/optional.hpp>

#include<OpenEXR/ImathMatrix.h>

namespace ramen
{

class corner_pin_node_t : public image_xform_node_t
{
public:

    corner_pin_node_t();

protected:

    corner_pin_node_t(const corner_pin_node_t& other) : image_xform_node_t(other) {}
    void operator=(const corner_pin_node_t&);

private:

    friend class corner_pin_manipulator_t;

    node_t *do_clone() const { return new corner_pin_node_t(*this);}

    virtual void do_create_params();
    virtual void do_create_manipulators();

    virtual boost::optional<Imath::M33d> do_calc_transform_matrix_at_time( float time, int subsample = 1) const;

    void get_corners( boost::array<Imath::V2d, 4>& pts, float scale = 1.0f) const;
    void get_corners_at_time( boost::array<Imath::V2d, 4>& pts, float time, float scale = 1.0f) const;

    bool quad_is_convex( const boost::array<Imath::V2d, 4>& quad) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
