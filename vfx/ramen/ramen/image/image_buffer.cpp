// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/image_buffer.hpp>

#include<iostream>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/memory/image_memory_manager.hpp>

namespace ramen
{
namespace image
{

namespace
{

struct array_deleter
{
    array_deleter( std::size_t size) : size_( size)
    {
        assert( size_ != 0 && "Passing 0 size to array deleter");
    }

    void operator()( unsigned char *p) const
    {
        image_memory_manager_t::Instance().deallocate( p, size_);

        if( p != 0)
            ++image_buffer_t::num_ib_freed;
    }

private:

    std::size_t size_;
};

} // namespace

int image_buffer_t::num_ib_allocs = 0;
int image_buffer_t::num_ib_freed = 0;

image_buffer_t::image_buffer_t() : cached_pixels_( false), channels_(0) {}

image_buffer_t::image_buffer_t( int width, int height, int channels) : cached_pixels_( false), channels_( channels)
{
    assert( ( channels == 1 || channels == 4) && "image_buffer_t: only 1 and 4 channels images supported");

    bounds_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width-1, height-1));

    if( bounds_.isEmpty())
	return;
    
    alloc_pixels();
}

image_buffer_t::image_buffer_t( const Imath::Box2i& bounds, int channels) : bounds_( bounds), cached_pixels_( false), channels_( channels)
{
    assert( ( channels == 1 || channels == 4) && "image_buffer_t: only 1 and 4 channels images supported");

    if( bounds_.isEmpty())
	return;

    alloc_pixels();
}

void image_buffer_t::alloc_pixels()
{
    std::size_t size = (( width() * height() * channels()) + 3) * sizeof( float);
    pixels_.reset( image_memory_manager_t::Instance().allocate( size), array_deleter( size));
    ++num_ib_allocs;    
}

int image_buffer_t::channels() const { return channels_;}

void image_buffer_t::clear()
{
    cached_pixels_ = false;
    bounds_ = Imath::Box2i();
    pixels_.reset();
}

const_image_view_t image_buffer_t::const_rgba_view() const
{
    assert( !empty() && "Trying to get a view from an empty image");
    assert( channels() == 4);

    return boost::gil::interleaved_view<const_pixel_ptr_t>( width(), height(), reinterpret_cast<const_pixel_ptr_t>( aligned_ptr()),
                                                            width() * channels() * sizeof( float));
}

const_gray_image_view_t image_buffer_t::const_gray_view() const
{
    assert( !empty() && "Trying to get a view from an empty image");
    assert( channels() == 1);

    return boost::gil::interleaved_view<const_gray_pixel_ptr_t>( width(), height(), reinterpret_cast<const_gray_pixel_ptr_t>( aligned_ptr()),
                                                                width() * channels() * sizeof( float));
}

const_image_view_t image_buffer_t::const_rgba_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( const_rgba_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
							area.size().x+1, area.size().y+1);
}

const_gray_image_view_t image_buffer_t::const_gray_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( const_gray_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
							area.size().x+1, area.size().y+1);
}

image_view_t image_buffer_t::rgba_view() const
{
    assert( !empty()  && "Trying to get a view of an empty image");
    assert( !cached() && "Trying to get a non const view of an image cached");
    assert( channels() == 4);

    return boost::gil::interleaved_view<pixel_ptr_t>( width(), height(), reinterpret_cast<pixel_ptr_t>( aligned_ptr()),
                                                        width() * channels() * sizeof( float));

}

gray_image_view_t image_buffer_t::gray_view() const
{
    assert( !empty()  && "Trying to get a view of an empty image");
    assert( !cached() && "Trying to get a non const view of an image cached");
    assert( channels() == 1);
    
    return boost::gil::interleaved_view<gray_pixel_ptr_t>( width(), height(), reinterpret_cast<gray_pixel_ptr_t>( aligned_ptr()), 
                                                            width() * channels() * sizeof( float));
}

image_view_t image_buffer_t::rgba_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( rgba_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
							area.size().x+1, area.size().y+1);
}

gray_image_view_t image_buffer_t::gray_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( gray_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
							area.size().x+1, area.size().y+1);
}

void image_buffer_t::check_area_inside_image( const Imath::Box2i& area) const
{
    if( isInside( bounds_, area))
        return; // everything OK

    // Not OK, print some debug info and exit
    std::cout << "Trying to access pixels outside buffer\n";
    std::cout << "Area = " << area << " Bounds = " << bounds_ << "\n";
    abort();
}

bool image_buffer_t::operator==( const image_buffer_t& other) const
{
    return ( bounds() == other.bounds()) && ( pixels_ == other.pixels_);
}

bool image_buffer_t::operator!=( const image_buffer_t& other) const
{
    return ( bounds() != other.bounds()) || ( pixels_ != other.pixels_);
}

} // namespace
} // namespace
