// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/image_accumulator.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace image
{

image_accumulator_t::image_accumulator_t( const image_view_t& view) : view_( view)
{
    defined_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( view.width()-1, view.height()-1));
}

image_accumulator_t::image_accumulator_t( const image_view_t& view, const Imath::Box2i defined) : view_( view)
{
    defined_ = defined;
}

void image_accumulator_t::clear() { clear_image( view_);}

void image_accumulator_t::accumulate( const const_image_view_t& v, float factor)
{
    assert( v.dimensions() == view_.dimensions());
    madd_images( view_, v, factor, view_);
}

void image_accumulator_t::accumulate( const const_image_view_t& v, const Imath::Box2i& area, float factor)
{
    Imath::Box2i isect = intersect( area, defined_);

    if( isect.isEmpty())
        return;

    image_view_t dst_view = boost::gil::subimage_view( view_, isect.min.x - defined_.min.x, isect.min.y - defined_.min.y,
						      isect.size().x+1, isect.size().y+1);

    const_image_view_t src_view = boost::gil::subimage_view( v, isect.min.x - area.min.x, isect.min.y - area.min.y,
							    isect.size().x+1, isect.size().y+1);

    madd_images( dst_view, src_view, factor, dst_view);
}

void image_accumulator_t::multiply( float f) { mul_image_scalar( view_, f, view_);}

// gray image versions

gray_image_accumulator_t::gray_image_accumulator_t( const gray_image_view_t& view) : view_( view)
{
    defined_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( view.width()-1, view.height()-1));
}

gray_image_accumulator_t::gray_image_accumulator_t( const gray_image_view_t& view, const Imath::Box2i defined) : view_( view)
{
    defined_ = defined;
}

void gray_image_accumulator_t::clear()
{
    boost::gil::fill_pixels( view_, boost::gil::gray32f_pixel_t( 0));
}

void gray_image_accumulator_t::accumulate( const const_gray_image_view_t& v, float factor)
{
    assert( v.dimensions() == view_.dimensions());
    madd_gray_images( view_, v, factor, view_);
}

void gray_image_accumulator_t::accumulate( const const_gray_image_view_t& v, const Imath::Box2i& area, float factor)
{
    Imath::Box2i isect = intersect( area, defined_);

    if( isect.isEmpty())
        return;

    gray_image_view_t dst_view = boost::gil::subimage_view( view_, isect.min.x - defined_.min.x, isect.min.y - defined_.min.y,
                                                            isect.size().x+1, isect.size().y+1);

    const_gray_image_view_t src_view = boost::gil::subimage_view( v, isect.min.x - area.min.x, isect.min.y - area.min.y,
                                                                    isect.size().x+1, isect.size().y+1);

    madd_gray_images( dst_view, src_view, factor, dst_view);
}

void gray_image_accumulator_t::multiply( float f)
{ 
    mul_gray_image_scalar( view_, f, view_);
}

} // namespace
} // namespace
