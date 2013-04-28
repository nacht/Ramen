// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RESIZE_HPP
#define RAMEN_IMAGE_GENERIC_RESIZE_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<OpenEXR/ImathBox.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

template<class Sampler>
class resize_fn
{
public:

    resize_fn( const const_image_view_t& src, const image_view_t& dst) : src_(src), dst_( dst), s_( src)
    {
        yscale_ = src_.height() / dst_.height();
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
	for( int j = r.begin(); j < r.end(); ++j)
	{
            image_view_t::x_iterator dst_it( dst_.row_begin( j));

            Imath::V2f p ( 0, j * yscale_);
            float xinc( src_.width() / dst_.width());

	    for( int i = 0; i < dst_.width(); ++i)
	    {
		*dst_it++ = s_( p);
                p.x += xinc;
	    }
	}
    }

private:

    Sampler s_;
    const const_image_view_t& src_;
    const image_view_t& dst_;
    float yscale_;
};

} // detail

template<class Sampler>
void resize( const const_image_view_t& src, const image_view_t& dst)
{
    tbb::parallel_for( tbb::blocked_range<int>( 0, dst.height() + 1), detail::resize_fn<Sampler>( src, dst), tbb::auto_partitioner());
}

void resize_nearest( const const_image_view_t& src, const image_view_t& dst);
void resize_bilinear( const const_image_view_t& src, const image_view_t& dst);
void resize_bicubic( const const_image_view_t& src, const image_view_t& dst);

} // namespace
} // namespace
} // namespace

#endif
