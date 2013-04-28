// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/convolve33.hpp>

#include<algorithm>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct convolve33_fun
{
    convolve33_fun( const image::const_image_view_t& src, const image::image_view_t& dst, const Imath::M33f& k) : src_( src), dst_( dst)
    {
        k_ = k;

        inv_sum_weights_ = 0.0;

        for( int j = 0; j < 3; ++j)
            for( int i = 0; i < 3; ++i)
                inv_sum_weights_ += k_[j][i];

        if( inv_sum_weights_ != 0)
            inv_sum_weights_ = 1.0f / inv_sum_weights_;
        else
            inv_sum_weights_ = 1.0f;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        for( int j = r.begin(); j < r.end(); ++j)
        {
            image::const_image_view_t::x_iterator top_it( src_.row_begin( std::max( j - 1, 0)));
            image::const_image_view_t::x_iterator center_it( src_.row_begin( j));
            image::const_image_view_t::x_iterator bottom_it( src_.row_begin( std::min( j + 1, (int) src_.height() - 1)));
            image::image_view_t::x_iterator dst_it( dst_.row_begin( j));

            image::pixel_t acc;

            // first pixel
            for( int k = 0; k < 3; ++k)
            {
                acc[k]  = (*top_it)[k] * k_[0][0];
                acc[k] += (*top_it)[k] * k_[0][1];
                acc[k] += top_it[1][k] * k_[0][2];

                acc[k] += (*center_it)[k] * k_[1][0];
                acc[k] += (*center_it)[k] * k_[1][1];
                acc[k] += center_it[1][k] * k_[1][2];

                acc[k] += (*bottom_it)[k] * k_[2][0];
                acc[k] += (*bottom_it)[k] * k_[2][1];
                acc[k] += bottom_it[1][k] * k_[2][2];

                acc[k] /= inv_sum_weights_;
            }

            *dst_it++ = acc;
            ++top_it;
            ++center_it;
            ++bottom_it;

            // loop
            for( int i = 1; i < src_.width()-1; ++i)
            {
                for( int k = 0; k < 3; ++k)
                {
                    acc[k]  = top_it[-1][k] * k_[0][0];
                    acc[k] += ( *top_it)[k] * k_[0][1];
                    acc[k] += top_it[ 1][k] * k_[0][2];

                    acc[k] += center_it[-1][k] * k_[1][0];
                    acc[k] += ( *center_it)[k] * k_[1][1];
                    acc[k] += center_it[ 1][k] * k_[1][2];

                    acc[k] += bottom_it[-1][k] * k_[2][0];
                    acc[k] += ( *bottom_it)[k] * k_[2][1];
                    acc[k] += bottom_it[ 1][k] * k_[2][2];

                    acc[k] /= inv_sum_weights_;
                }

                *dst_it++ = acc;
                ++top_it;
                ++center_it;
                ++bottom_it;
            }

            // last pixel
            for( int k = 0; k < 3; ++k)
            {
                acc[k]  = top_it[-1][k] * k_[0][0];
                acc[k] += ( *top_it)[k] * k_[0][1];
                acc[k] += ( *top_it)[k] * k_[0][2];

                acc[k] += center_it[-1][k] * k_[1][0];
                acc[k] += ( *center_it)[k] * k_[1][1];
                acc[k] += ( *center_it)[k] * k_[1][2];

                acc[k] += bottom_it[-1][k] * k_[2][0];
                acc[k] += ( *bottom_it)[k] * k_[2][1];
                acc[k] += ( *bottom_it)[k] * k_[2][2];

                acc[k] /= inv_sum_weights_;
            }

            *dst_it = acc;
        }
    }

private:

    const image::const_image_view_t& src_;
    const image::image_view_t& dst_;
    Imath::M33f k_;
    float inv_sum_weights_;
};

} // detail

void convolve33( const const_image_view_t& src, const image_view_t& dst, const Imath::M33f& k)
{
    assert( src.dimensions() == dst.dimensions() && "convolve33: src and dst views must have the same size");
    tbb::parallel_for( tbb::blocked_range<int>( 0, dst.height()), detail::convolve33_fun( src, dst, k), tbb::auto_partitioner());
}

} // namespace
} // namespace
} // namespace
