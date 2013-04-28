//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_HALF_HPP
#define GIL_EXTENSION_HALF_HPP

#include<OpenEXR/half.h>

#include<boost/type_traits/is_class.hpp>

#include<boost/gil/channel.hpp>
#include<boost/gil/channel_algorithm.hpp>
#include<boost/gil/typedefs.hpp>

namespace boost
{

// I can't remember why I did this. Is it really needed?
template<> struct is_class<half> : public false_type{};

namespace gil
{

struct half_zero { static half apply() { return half( 0.0f);}};
struct half_one  { static half apply() { return half( 1.0f);}};

typedef scoped_channel_value<half,half_zero,half_one> bits16f;

// channel conversion
template <> struct channel_converter_unsigned<bits16f,bits8> : public std::unary_function<bits16f,bits8>
{
    bits8 operator()(bits16f x) const { return static_cast<bits8>( half(x) * 255.0f + 0.5f);}
};

template <> struct channel_converter_unsigned<bits8,bits16f> : public std::unary_function<bits8,bits16f>
{
    bits16f operator()(bits8   x) const { return static_cast<bits16f>( half(x) / 255.0f);}
};

template <> struct channel_converter_unsigned<bits16f,bits16> : public std::unary_function<bits16f,bits16>
{
    bits16 operator()(bits16f x) const { return static_cast<bits16>( half(x) * 65535.0f + 0.5f);}
};

template <> struct channel_converter_unsigned<bits16,bits16f> : public std::unary_function<bits16,bits16f>
{
    bits16f operator()(bits16  x) const { return static_cast<bits16f>( float(x) / 65535.0f);}
};

// floating point
template <> struct channel_converter_unsigned<bits32f,bits16f> : public std::unary_function<bits32f,bits16f>
{
    bits16f operator()(bits32f  x) const { return bits16f( half(x));}
};

template <> struct channel_converter_unsigned<bits16f,bits32f> : public std::unary_function<bits16f,bits32f>
{
    bits32f operator()(bits16f  x) const { return bits32f( half(x));}
};

template<> struct channel_multiplier_unsigned<bits16f> : public std::binary_function<bits16f,bits16f,bits16f>
{
    bits16f operator()(bits16f a, bits16f b) const { return bits16f( half(a) * half(b));}
};

// typedefs
GIL_DEFINE_BASE_TYPEDEFS(16f,bgr)
GIL_DEFINE_BASE_TYPEDEFS(16f,argb)
GIL_DEFINE_BASE_TYPEDEFS(16f,abgr)
GIL_DEFINE_BASE_TYPEDEFS(16f,bgra)

GIL_DEFINE_ALL_TYPEDEFS(16f,gray)
GIL_DEFINE_ALL_TYPEDEFS(16f,rgb)
GIL_DEFINE_ALL_TYPEDEFS(16f,rgba)
GIL_DEFINE_ALL_TYPEDEFS(16f,cmyk)

template <int N> struct devicen_t;
template <int N> struct devicen_layout_t;

GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev2n, devicen_t<2>, devicen_layout_t<2>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev3n, devicen_t<3>, devicen_layout_t<3>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev4n, devicen_t<4>, devicen_layout_t<4>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev5n, devicen_t<5>, devicen_layout_t<5>)

} // namespace
} // namespace

#endif
