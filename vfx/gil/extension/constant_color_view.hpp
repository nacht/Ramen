//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_CONSTANT_COLOR_VIEW_HPP
#define GIL_EXTENSION_CONSTANT_COLOR_VIEW_HPP

#include<boost/gil/utilities.hpp>
#include<boost/gil/image_view.hpp>
#include<boost/gil/virtual_locator.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

template<class Pixel>
struct constant_color_fn
{
	typedef point2<ptrdiff_t>	 point_t;
    typedef constant_color_fn	 const_t;
    typedef Pixel				 value_type;
    typedef value_type           reference;
    typedef value_type           const_reference;
    typedef point_t              argument_type;
    typedef reference            result_type;
    BOOST_STATIC_CONSTANT( bool, is_mutable=false);

    constant_color_fn(const point_t& sz, const Pixel& color) : size_(sz), color_(color) {}

    result_type operator()(const point_t& p) const { return color_;}
	
private:

	point_t size_;
	Pixel color_;
};

} // detail

template<class Pixel>
image_view< virtual_2d_locator<detail::constant_color_fn<Pixel> ,false> >
make_constant_color_view( const point2<ptrdiff_t>& dims, const Pixel& color)
{
typedef virtual_2d_locator<detail::constant_color_fn<Pixel> ,false> locator_t;
typedef image_view<locator_t> constant_color_view_t;

	return constant_color_view_t(dims, locator_t( point2<ptrdiff_t>(0,0), point2<ptrdiff_t>(1,1), 
									detail::constant_color_fn<Pixel>(dims, color)));
}

} // namespace
} // namespace

#endif
