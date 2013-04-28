//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_RGBA_PREMULT_COLOR_CONVERT_HPP
#define	GIL_EXTENSION_RGBA_PREMULT_COLOR_CONVERT_HPP

#include<boost/gil/color_convert.hpp>

namespace boost
{
namespace gil
{

struct rgba_premultiplied_color_converter
{
    template<class SrcP, class DstP>
    void operator()( const SrcP& src, DstP& dst)
    {
        // BOOST_STATIC_ASSERT( SrcP::layout == rgba_t);
        typedef typename channel_type<P1>::type channel_type;        
        cc_( pixel<channel_type, rgb_layout_t>( get_color(src,red_t()), get_color(src,green_t()), get_color(src,blue_t())) ,dst);
    }

private:
    
    default_color_converter cc_;
};

} // namespace
} // namespace

#endif

