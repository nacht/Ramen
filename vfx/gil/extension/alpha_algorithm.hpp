//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_ALPHA_ALGORITHM_HPP
#define GIL_EXTENSION_ALPHA_ALGORITHM_HPP

#include<boost/gil/channel_algorithm.hpp>
#include<boost/gil/rgba.hpp>

#include<boost/gil/utilities.hpp>
#include<boost/gil/image_view.hpp>
#include<boost/gil/virtual_locator.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

template<class Channel>
Channel chan_divide( Channel c, Channel a)
{	
    float tmp = (float) c / a;
    return Channel( tmp);
}

struct alpha_premult_t
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
        typename PixelRef::value_type r;
        get_color( r, red_t())		= channel_multiply( get_color( p, red_t()), get_color( p, alpha_t()));
        get_color( r, green_t())	= channel_multiply( get_color( p, green_t()), get_color( p, alpha_t()));
        get_color( r, blue_t())		= channel_multiply( get_color( p, blue_t()), get_color( p, alpha_t()));
        get_color( r, alpha_t())	= get_color( p, alpha_t());
        return r;
    }
};

struct alpha_unpremult_t
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
        typename PixelRef::value_type r;
        typename PixelRef::channel_t a;

        a = get_color( p, alpha_t());

        if( a != 0)
        {
            get_color( r, red_t())	= chan_divide( get_color( p, red_t()), a);
            get_color( r, green_t())	= chan_divide( get_color( p, green_t()), a);
            get_color( r, blue_t())	= chan_divide( get_color( p, blue_t()), a);
            get_color( r, alpha_t())	= a;
        }
        else
        {
            get_color( r, red_t())	= 0;
            get_color( r, green_t())	= 0;
            get_color( r, blue_t())	= 0;
            get_color( r, alpha_t())	= 0;
        }

        return r;
    }
};

template<class Pixel>
struct checks_fn
{
    typedef point2<ptrdiff_t>	point_t;
    typedef checks_fn		const_t;
    typedef Pixel		value_type;
    typedef value_type          reference;
    typedef value_type          const_reference;
    typedef point_t             argument_type;
    typedef reference           result_type;
    BOOST_STATIC_CONSTANT( bool, is_mutable=false);

    checks_fn(const point_t& sz, std::size_t check_size, 
		const Pixel& color0, const Pixel& color1) : size_(sz), check_size_( check_size), color0_(color0), color1_(color1)
    {
    }

    result_type operator()(const point_t& p) const
    {
        ptrdiff_t row = p.y / check_size_;
        ptrdiff_t col = p.x / check_size_;

        if( row & 1)
        {
                if( col & 1)
                        return color1_;
                else
                        return color0_;
        }
        else
        {
                if( col & 1)
                        return color0_;
                else
                        return color1_;
        }

        return color0_;
    }
	
private:

	point_t size_;
	std::size_t check_size_;
	Pixel color0_, color1_;
};

} // namespace

template<class RGBAView>
void premult_alpha( const RGBAView& dst) { for_each_pixel( dst, detail::alpha_premult_t());}

template<class RGBAView>
void unpremult_alpha( const RGBAView& dst) { for_each_pixel( dst, detail::alpha_unpremult_t());}

template<class RGBAView, class Channel>
void set_alpha( const RGBAView& dst, Channel value)
{
   for( int y = 0; y < dst.dimensions().y; ++y )
   {
        typename RGBAView::x_iterator dst_it = dst.row_begin( y );

        for( int x = 0; x < dst.dimensions().x; ++x )
                get_color( dst_it[x], alpha_t() ) = value;
   }
}

template<class RGBAView, class GrayView>
void copy_alpha( const RGBAView& dst, const GrayView& src)
{
   for( int y = 0; y < dst.dimensions().y; ++y )
   {
        typename GrayView::x_iterator src_it = src.row_begin( y );
        typename RGBAView::x_iterator dst_it = dst.row_begin( y );

        for( int x = 0; x < dst.dimensions().x; ++x )
                get_color( dst_it[x], alpha_t() ) = src_it[x][0];
   }
}

template<class RGBAView>
void invert_alpha( const RGBAView& dst)
{
   for( int y = 0; y < dst.dimensions().y; ++y )
   {
        typename RGBAView::x_iterator dst_it = dst.row_begin( y );

        for( int x = 0; x < dst.dimensions().x; ++x )
                get_color( dst_it[x], alpha_t() ) = channel_invert( get_color( dst_it[x], alpha_t()));
   }
}

template<class Pixel>
image_view< virtual_2d_locator<detail::checks_fn<Pixel> ,false> >
make_checks_view( const point2<ptrdiff_t>& dims, std::size_t checks_size, const Pixel& color0, const Pixel& color1)
{
typedef virtual_2d_locator<detail::checks_fn<Pixel> ,false> locator_t;
typedef image_view<locator_t> checks_view_t;

    return checks_view_t( dims, locator_t( point2<ptrdiff_t>(0,0), point2<ptrdiff_t>(1,1),
				detail::checks_fn<Pixel>(dims, checks_size, color0, color1)));
}

} // namespace
} // namespace

#endif

