// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_IMAGE_BUFFER_HPP
#define	RAMEN_IMAGE_IMAGE_BUFFER_HPP

#include<boost/shared_array.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/cstdint.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

class image_buffer_t
{
public:

    image_buffer_t();
    image_buffer_t( int width, int height, int channels = 4);
    image_buffer_t( const Imath::Box2i& bounds, int channels = 4);

    int width() const	{ return bounds_.size().x + 1;}
    int height() const	{ return bounds_.size().y + 1;}
    int channels() const;

    bool empty() const { return !pixels_;}

    void clear();

    const Imath::Box2i& bounds() const { return bounds_;}

    // flags
    bool cached() const		{ return cached_pixels_;}
    void set_cached( bool b)	{ cached_pixels_ = true;}

    // const views
    const_image_view_t const_rgba_view() const;
    const_gray_image_view_t const_gray_view() const;

    const_image_view_t const_rgba_subimage_view( const Imath::Box2i& area) const;
    const_gray_image_view_t const_gray_subimage_view( const Imath::Box2i& area) const;

    // non const views
    image_view_t rgba_view() const;
    gray_image_view_t gray_view() const;

    image_view_t rgba_subimage_view( const Imath::Box2i& area) const;
    gray_image_view_t gray_subimage_view( const Imath::Box2i& area) const;

    bool operator==( const image_buffer_t& other) const;
    bool operator!=( const image_buffer_t& other) const;

private:

    void alloc_pixels();

    unsigned char *aligned_ptr() const
    {
        // return a 16 bytes aligned ptr (for sse)
        unsigned char *mem = pixels_.get();
        unsigned char *aligned = mem;

        if( (boost::uint64_t ) mem & 0xf)
            aligned = mem + 4 - (((boost::uint64_t ) mem & 0xf) >> 2);

        return aligned;
    }

    // for debugging help
    void check_area_inside_image( const Imath::Box2i& area) const;

    Imath::Box2i bounds_;
    int channels_;
    boost::shared_array<unsigned char> pixels_;

    bool cached_pixels_;

// debugging
public:

    static int num_ib_allocs;
    static int num_ib_freed;
};

} // namespace
} // namespace

#endif
