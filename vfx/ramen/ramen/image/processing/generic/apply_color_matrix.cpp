// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/apply_color_matrix.hpp>

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

class apply_color_matrix_fn
{
public:

    apply_color_matrix_fn( const const_image_view_t& src, const image_view_t& dst, const Imath::M44f& m) : src_(src), dst_( dst), m_( m) {}

    void operator()( const tbb::blocked_range<int>& range) const
    {
	for( int j = range.begin(); j < range.end(); ++j)
	{
            const_image_view_t::x_iterator src_it( src_.row_begin( j));
            image_view_t::x_iterator dst_it( dst_.row_begin( j));

	    for( int i = 0; i < src_.width(); ++i)
	    {
                float r = boost::gil::get_color( *src_it, boost::gil::red_t());
                float g = boost::gil::get_color( *src_it, boost::gil::green_t());
                float b = boost::gil::get_color( *src_it, boost::gil::blue_t());
                float a = boost::gil::get_color( *src_it, boost::gil::alpha_t());

                float xr = r * m_[0][0] + g * m_[1][0] + b * m_[2][0] + m_[3][0];
                float xg = r * m_[0][1] + g * m_[1][1] + b * m_[2][1] + m_[3][1];
                float xb = r * m_[0][2] + g * m_[1][2] + b * m_[2][2] + m_[3][2];

                *dst_it = image::pixel_t( std::max( xr, 0.0f),
                                          std::max( xg, 0.0f),
                                          std::max( xb, 0.0f),
                                          a);
                ++src_it;
                ++dst_it;
	    }
	}
    }

private:

    const const_image_view_t& src_;
    const image_view_t& dst_;
    Imath::M44f m_;
};

} // detail

void apply_color_matrix( const const_image_view_t& src, const image_view_t& dst, const Imath::M44f& m)
{
    assert( src.dimensions() == dst.dimensions() && "Apply color matrix, src and dst must have the same size");
    tbb::parallel_for( tbb::blocked_range<int>( 0, src.height()), detail::apply_color_matrix_fn( src, dst, m), tbb::auto_partitioner());
}


} // namespace
} // namespace
} // namespace
