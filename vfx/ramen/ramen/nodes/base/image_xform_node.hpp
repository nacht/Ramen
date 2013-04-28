// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_XFORM_NODE_HPP
#define RAMEN_XFORM_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<vector>

#include<boost/optional.hpp>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/params/motion_blur_param.hpp>

namespace ramen
{

class image_xform_node_t : public image_node_t
{
public:

    enum filter_type
    {
        filter_point = 0,
        filter_bilinear,
        filter_bicubic
    };

    image_xform_node_t();

protected:

    image_xform_node_t(const image_xform_node_t& other) : image_node_t(other) { }
    void operator=(const image_xform_node_t&);

    const motion_blur_param_t *motion_blur_param() const;

    image_node_t *get_img_source();
    const image_node_t *get_img_source_and_xform_list( std::vector<const image_xform_node_t*>& xforms) const;

    boost::optional<Imath::M33d> global_matrix_at_time( float time, const std::vector<const image_xform_node_t*>& xforms, int subsample = 1) const;

private:

    virtual boost::optional<Imath::M33d> calc_transform_matrix_at_time( float time, int subsample = 1) const;
    virtual boost::optional<Imath::M33d> do_calc_transform_matrix_at_time( float time, int subsample = 1) const = 0;

    virtual filter_type get_filter_type() const;

    Imath::Box2i projective_transform_box( const Imath::Box2i& box, const Imath::M33d& m, int step, bool round_up = false) const;

    virtual void do_calc_domain( const render::render_context_t& context);
    virtual void do_calc_bounds( const render::render_context_t& context);

    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_recursive_process( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);

    void do_process( const image_node_t *src, const image::image_view_t& dst, const boost::optional<Imath::M33d>& xf);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::image_xform_node_t)

#endif
