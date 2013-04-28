//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_LUMINANCE_VIEW_HPP
#define GIL_EXTENSION_LUMINANCE_VIEW_HPP

#include<boost/gil/gil_all.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

template <typename SrcP>
class luminance_deref_fn
{
    BOOST_STATIC_CONSTANT( bool, is_mutable=pixel_is_reference<SrcP>::value && pixel_reference_is_mutable<SrcP>::value);

    typedef typename remove_reference<SrcP>::type src_pixel_t;
    typedef typename channel_type<src_pixel_t>::type channel_t;
    typedef typename src_pixel_t::const_reference const_ref_t;
    typedef typename pixel_reference_type<channel_t, gray_layout_t, false, is_mutable>::type ref_t;

public:

    typedef luminance_deref_fn<const_ref_t>                                         	const_t;
    typedef typename pixel_value_type<channel_t,gray_layout_t>::type                    value_type;
    typedef typename pixel_reference_type<channel_t,gray_layout_t,false,false>::type    const_reference;
    typedef SrcP                                                                        argument_type;
    typedef typename mpl::if_c<is_mutable, ref_t, value_type>::type                     reference;
    typedef reference                                                                   result_type;

    luminance_deref_fn() {}

    template <typename P>
    luminance_deref_fn( const luminance_deref_fn<P>& d) {}

    result_type operator()( argument_type srcP) const
    {
        return result_type( ( 0.33f * get_color( srcP, red_t()))  +
                            ( 0.33f * get_color( srcP, green_t()) +
                            ( 0.33f * get_color( srcP, blue_t())));
    }
};

template <typename View> 
struct __luminance_view<View>
{
private:

    typedef luminance_deref_fn<typename View::reference> deref_t;
    typedef typename View::template add_deref<deref_t>   AD;

public:

    typedef typename AD::type type;

    static type make(const View& src) { return AD::make(src, deref_t());}
};

} // namespace detail

template <typename View>
struct luminance_view_type
{
private:

    GIL_CLASS_REQUIRE( View, boost::gil, ImageViewConcept)
    typedef detail::__luminance_view<View> VB;

public:

    typedef typename VB::type type;
    static type make( const View& src) { return VB::make(src);}
};

template <typename View>
typename luminance_view_type<View>::type luminance_view( const View& src)
{
    return luminance_view_type<View>::make( src);
}

} // namespace
} // namespace

#endif

