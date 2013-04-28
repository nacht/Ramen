// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_SHAPE_RENDERER_HPP
#define	RAMEN_QSHAPE_SHAPE_RENDERER_HPP

#include<boost/gil/gil_all.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/image/agg_renderers.hpp>

#include<agg/agg_renderer_scanline.h>
#include<agg/agg_rasterizer_scanline_aa.h>
#include<agg/agg_scanline_u.h>
#include<agg/agg_path_storage.h>
#include<agg/agg_bounding_rect.h>

#include<ramen/nodes/qshape/shape.hpp>

#include<ramen/render/render_context.hpp>

namespace ramen
{
namespace qshape
{

class shape_renderer_t
{
    typedef agg_gray32f_renderer_t<boost::gil::gray32f_view_t> ren_base_type;
    typedef ren_base_type::color_type color_type;
    typedef agg::renderer_scanline_aa_solid<ren_base_type> renderer_type;

public:

    shape_renderer_t( float t, const Imath::Box2i& area, int subsample);

    void set_time( float t) { time_ = t;}

    const boost::gil::gray32fc_view_t view() const { return boost::gil::const_view( pixels_);}

    void clear();

    void render_shape( const shape_t& shape);

private:

    void render_and_blur_shape( const shape_t& shape);
    void render_shape( const shape_t& shape, renderer_type& ren, const Imath::V2i& offset, float color, float alpha);

    float time_;

    boost::gil::gray32f_image_t pixels_;

    ren_base_type ren_base_;
    agg::scanline_u8 sl_;
    agg::rasterizer_scanline_aa<> ras_;
    renderer_type ren_;

    agg::path_storage path_;

    Imath::Box2i area_;
    int subsample_;
};

} // namespace
} // namespace

#endif
