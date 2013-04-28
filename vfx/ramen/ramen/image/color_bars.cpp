// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/color_bars.hpp>

#include<boost/gil/gil_all.hpp>

namespace ramen
{
namespace image
{

namespace detail
{

struct color_bars_fn
{
	typedef color_bars_fn					const_t;
	typedef boost::gil::point2<ptrdiff_t>   point_t;
	typedef boost::gil::rgba32f_pixel_t		value_type;
	typedef value_type						reference;
	typedef value_type						const_reference;
	typedef point_t							argument_type;
	typedef reference						result_type;

	BOOST_STATIC_CONSTANT(bool, is_mutable=false);
	
	color_bars_fn() {}
	color_bars_fn( const point_t& sz) : width_( sz.x/7) {}

    pixel_t operator()( const point_t& p) const
	{
		int bar = p.x / width_;
						
		switch( bar)
		{
			case 0:
				return pixel_t( 1, 1, 1, 1);
			case 1:
				return pixel_t( 1, 1, 0, 1);
			case 2:
				return pixel_t( 0, 1, 1, 1);
			case 3:
				return pixel_t( 0, 1, 0, 1);
			case 4:
				return pixel_t( 1, 0, 1, 1);
			case 5:
				return pixel_t( 1, 0, 0, 1);
			default:
				return pixel_t( 0, 0, 1, 1);
		}
    }

	int width_;
};

} // detail

void make_color_bars( const image_view_t& view)
{
    typedef detail::color_bars_fn deref_t;
    typedef deref_t::point_t point_t;
    typedef boost::gil::virtual_2d_locator<deref_t,false> locator_t;
    typedef boost::gil::image_view<locator_t> my_virt_view_t;

    point_t dims( view.width(), view.height());
    my_virt_view_t bars( dims, locator_t( point_t(0,0), point_t(1,1), deref_t( dims)));
    boost::gil::copy_pixels( bars, view);
}

void make_color_bars( const image_view_t& view, const Imath::Box2i& domain, const Imath::Box2i& defined)
{
    typedef detail::color_bars_fn deref_t;
    typedef deref_t::point_t point_t;
    typedef boost::gil::virtual_2d_locator<deref_t,false> locator_t;
    typedef boost::gil::image_view<locator_t> my_virt_view_t;

    point_t dims( domain.size().x+1, domain.size().y+1);
    my_virt_view_t bars( dims, locator_t( point_t(0,0), point_t(1,1), deref_t( dims)));

    boost::gil::copy_pixels( boost::gil::subimage_view( bars, defined.min.x - domain.min.x,
								defined.min.y - domain.min.y,
								defined.size().x+1, defined.size().y+1), view);
}

} // namespace
} // namespace
