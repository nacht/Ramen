//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_RGB_FROM_RGBA_VIEW_HPP
#define GIL_EXTENSION_RGB_FROM_RGBA_VIEW_HPP

#include<boost/gil/gil_all.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

template<class View, bool IsAdjacent> struct __rgb_from_rgba_view_basic;

template<class View>
struct __rgb_from_rgba_view_basic<View,false>
{
    typedef typename view_type<typename channel_type<View>::type, rgb_layout_t, true, true, view_is_mutable<View>::value>::type type;

    static type make( const View& src)
    {
        typedef typename type::xy_locator                               locator_t;
        typedef typename type::x_iterator                               x_iterator_t;
        typedef typename iterator_adaptor_get_base<x_iterator_t>::type  x_iterator_base_t;

        x_iterator_base_t base_it( &( get_color( src( 0, 0), red_t())),
                                   &( get_color( src( 0, 0), green_t())),
                                   &( get_color( src( 0, 0), blue_t())));

        x_iterator_t sit( base_it, src.pixels().pixel_size());
        return type( src.dimensions(), locator_t( sit, src.pixels().row_size()));
    }
};

template<class View>
struct __rgb_from_rgba_view_basic<View,true>
{
    typedef typename view_type<typename channel_type<View>::type, rgb_layout_t, true, false, view_is_mutable<View>::value>::type type;

    static type make( const View& src)
    {
        typedef typename type::x_iterator x_iterator_t;

        return planar_rgb_view( src.width(), src.height(), (x_iterator_t) &( get_color( src( 0, 0), red_t())),
                                                            (x_iterator_t) &( get_color( src( 0, 0), green_t())),
                                                            (x_iterator_t) &( get_color( src( 0, 0), blue_t())),
                                                            src.pixels().row_size());
    }
};

template <typename View, bool IsBasic> struct __rgb_from_rgba_view;

// For basic (memory-based) views dispatch to __rgb_from_rgba_view_basic
template <typename View> struct __rgb_from_rgba_view<View,true>
{
private:

    typedef typename View::x_iterator src_x_iterator;

    // Determines whether the channels of a given pixel iterator are adjacent in memory.
    // Planar and grayscale iterators have channels adjacent in memory, whereas multi-channel interleaved and iterators with non-fundamental step do not.
    BOOST_STATIC_CONSTANT( bool, adjacent= !iterator_is_step<src_x_iterator>::value && is_planar<src_x_iterator>::value);

public:

    typedef typename __rgb_from_rgba_view_basic<View,adjacent>::type type;

    static type make( const View& src)
    {
        return __rgb_from_rgba_view_basic<View,adjacent>::make( src);
    }
};

template <typename SrcP>
class rgb_from_rgba_deref_fn
{
    BOOST_STATIC_CONSTANT( bool, is_mutable=pixel_is_reference<SrcP>::value && pixel_reference_is_mutable<SrcP>::value);

    typedef typename remove_reference<SrcP>::type src_pixel_t;
    typedef typename channel_type<src_pixel_t>::type channel_t;
    typedef typename src_pixel_t::const_reference const_ref_t;
    typedef typename pixel_reference_type<channel_t, rgb_layout_t, false, is_mutable>::type ref_t;

public:

    typedef rgb_from_rgba_deref_fn<const_ref_t>                                         const_t;
    typedef typename pixel_value_type<channel_t,rgb_layout_t>::type                     value_type;
    typedef typename pixel_reference_type<channel_t,rgb_layout_t,false,false>::type     const_reference;
    typedef SrcP                                                                        argument_type;
    typedef typename mpl::if_c<is_mutable, ref_t, value_type>::type                     reference;
    typedef reference                                                                   result_type;

    rgb_from_rgba_deref_fn() {}

    template <typename P>
    rgb_from_rgba_deref_fn( const rgb_from_rgba_deref_fn<P>& d) {}

    result_type operator()( argument_type srcP) const
    {
        return result_type( get_color( srcP, red_t()),
                            get_color( srcP, green_t()),
                            get_color( srcP, blue_t()));
    }
};

template <typename View> struct
__rgb_from_rgba_view<View,false>
{
private:

    typedef rgb_from_rgba_deref_fn<typename View::reference> deref_t;
    typedef typename View::template add_deref<deref_t>   AD;

public:

    typedef typename AD::type type;

    static type make(const View& src) { return AD::make(src, deref_t());}
};

} // namespace detail

template <typename View>
struct rgb_from_rgba_view_type
{
private:

    GIL_CLASS_REQUIRE( View, boost::gil, ImageViewConcept)
    typedef detail::__rgb_from_rgba_view<View,view_is_basic<View>::value> VB;

public:

    typedef typename VB::type type;
    static type make( const View& src) { return VB::make(src);}
};

template <typename View>
typename rgb_from_rgba_view_type<View>::type rgb_from_rgba_view( const View& src)
{
    return rgb_from_rgba_view_type<View>::make( src);
}

} // namespace
} // namespace

#endif

