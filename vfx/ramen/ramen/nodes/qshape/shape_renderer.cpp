// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/shape_renderer.hpp>

#include<agg/agg_conv_curve.h>
#include<agg/agg_conv_contour.h>

#include<boost/gil/algorithm.hpp>
#include<boost/gil/typedefs.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/image/box_blur_channel.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace qshape
{

namespace
{

struct composite_layer
{
public:

    composite_layer( float c, float o) : color_( c), opacity_( o) {}

    boost::gil::gray32f_pixel_t operator()( const boost::gil::gray32f_pixel_t& fg, const boost::gil::gray32f_pixel_t& bg) const
    {
        float a = fg[0] * opacity_;
        float a_inv = 1.0f - a;
        return boost::gil::gray32f_pixel_t( ( color_ * a) + ( bg[0] * a_inv));
    }

private:

    float color_;
    float opacity_;
};

} // unnamed

shape_renderer_t::shape_renderer_t( float t, const Imath::Box2i& area, int subsample) : area_( area), subsample_( subsample)
{
    assert( !area.isEmpty() && "Empty area rect in shape renderer");

    time_ = t;

    pixels_.recreate( area.size().x + 1, area.size().y + 1);

    ren_base_.set_view( boost::gil::view( pixels_));
    ren_base_.clear( color_type( 0.0f, 0.0f));

    ren_.attach( ren_base_);

    ras_.gamma( agg::gamma_none());
}

void shape_renderer_t::clear()
{
    boost::gil::fill_pixels( boost::gil::view( pixels_), boost::gil::gray32f_pixel_t( 0));
}

void shape_renderer_t::render_shape( const shape_t& shape)
{
    float blur = get_value_at_time<float>( shape.param( "blur"), time_) / (float) subsample_;
    
    if( blur != 0)
        render_and_blur_shape( shape);
    else
        render_shape( shape, ren_, area_.min, get_value_at_time<float>( shape.param( "color"), time_),
                        get_value_at_time<float>( shape.param( "opacity"), time_));
}

void shape_renderer_t::render_and_blur_shape( const shape_t& shape)
{
    // TODO: implement this properly
    float blur = get_value_at_time<float>( shape.param( "blur"), time_) / (float) subsample_;
    Imath::Box2f bbox( shape.bounding_box_at_time( time_, subsample_));

    // expand the buffer to account for blur
    bbox.min.x -= std::ceil( blur) + 1;
    bbox.min.y -= std::ceil( blur) + 1;
    bbox.max.x += std::ceil( blur) + 1;
    bbox.max.y += std::ceil( blur) + 1;

    Imath::Box2i buffer_area( Imath::V2i( std::floor( bbox.min.x), std::floor( bbox.min.y)),
                                Imath::V2i( std::ceil( bbox.max.x), std::ceil( bbox.max.y)));

    Imath::Box2i isect( intersect( area_, buffer_area));

    if( isect.isEmpty())
        return;

    // alloc a buffer
    boost::gil::gray32f_image_t buffer( buffer_area.size().x + 1, buffer_area.size().y + 1);

    ren_base_type ren_base( boost::gil::view( buffer));
    ren_base.clear( color_type( 0.0f, 0.0f));

    renderer_type ren;
    ren.attach( ren_base);

    render_shape( shape, ren, buffer_area.min, 1, 1);

    // blur the shape
    boost::gil::gray32f_image_t result( buffer_area.size().x + 1, buffer_area.size().y + 1);
    boost::gil::copy_pixels( boost::gil::const_view( buffer), boost::gil::view( result));
    image::box_blur( boost::gil::const_view( buffer), boost::gil::view( result), blur, blur, 1);

    // composite
    boost::gil::gray32fc_view_t src_view( boost::gil::subimage_view( boost::gil::const_view( result),
                                            isect.min.x - buffer_area.min.x, isect.min.y - buffer_area.min.y,
                                            isect.size().x + 1, isect.size().y + 1));

    boost::gil::gray32f_view_t dst_view( boost::gil::subimage_view( boost::gil::view( pixels_),
                                            isect.min.x - area_.min.x, isect.min.y - area_.min.y,
                                            isect.size().x + 1, isect.size().y + 1));

    boost::gil::tbb_transform_pixels( src_view, dst_view, dst_view, composite_layer( get_value_at_time<float>( shape.param( "color"), time_),
                                                                            get_value_at_time<float>( shape.param( "opacity"), time_)));
}

void shape_renderer_t::render_shape( const shape_t& shape, renderer_type& ren, const Imath::V2i& offset, float color, float alpha)
{
    ras_.reset();
    path_.remove_all();

    shape.convert_to_path_storage( path_, time_, subsample_, offset);
    agg::conv_curve<agg::path_storage> cpath( path_);

    float grow = get_value_at_time<float>( shape.param( "grow"), time_) / subsample_;

    if( grow != 0)
    {
        agg::conv_contour<agg::conv_curve<agg::path_storage> > contour( cpath);
        shape.set_conv_contour_options( contour);
        contour.width( -grow);
        ras_.add_path( contour);
    }
    else
        ras_.add_path( cpath);

    ras_.filling_rule( agg::fill_non_zero);
    ren.color( color_type( color, alpha));
    agg::render_scanlines( ras_, sl_, ren);
}

} // namespace
} // namespace
