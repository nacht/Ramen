//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_ALGORITHM_TRANSFORM3_HPP
#define GIL_EXTENSION_ALGORITHM_TRANSFORM3_HPP

#include<boost/gil/algorithm.hpp>

namespace boost
{
namespace gil
{

// transform pixels for 3 inputs and one destination
template <typename View1, typename View2, typename View3, typename View4, typename F>
GIL_FORCEINLINE F transform_pixels(const View1& src1, const View2& src2, const View3& src3, const View4& dst, F fun)
{
	for (std::ptrdiff_t y=0; y<dst.height(); ++y)
	{
		typename View1::x_iterator srcIt1=src1.row_begin(y);
		typename View2::x_iterator srcIt2=src2.row_begin(y);
		typename View3::x_iterator srcIt3=src3.row_begin(y);
		typename View4::x_iterator dstIt=dst.row_begin(y);
        
		for( std::ptrdiff_t x=0; x<dst.width(); ++x)
			dstIt[x]=fun(srcIt1[x],srcIt2[x],srcIt3[x]);
	}
    
	return fun;
}

template <typename View1, typename View2, typename View3, typename View4, typename F>
GIL_FORCEINLINE F transform_pixel_positions(const View1& src1,const View2& src2,const View3& src3, const View4& dst, F fun)
{
    assert(src1.dimensions()==dst.dimensions());
    assert(src2.dimensions()==dst.dimensions());
    assert(src3.dimensions()==dst.dimensions());
	
    typename View1::xy_locator loc1=src1.xy_at(0,0);
    typename View2::xy_locator loc2=src2.xy_at(0,0);
    typename View3::xy_locator loc3=src3.xy_at(0,0);
	
    for (std::ptrdiff_t y=0; y<src1.height(); ++y)
    {
	typename View4::x_iterator dstIt=dst.row_begin(y);

	for(std::ptrdiff_t x=0; x<src1.width(); ++x, ++loc1.x(), ++loc2.x(), ++loc3.x())
	    dstIt[x]=fun(loc1,loc2,loc3);

	loc1.x()-=src1.width(); ++loc1.y();
	loc2.x()-=src2.width(); ++loc2.y();
	loc3.x()-=src3.width(); ++loc3.y();
    }
    return fun;
}

} // namespace
} // namespace

#endif
