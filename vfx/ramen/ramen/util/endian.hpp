// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#ifndef RAMEN_UTIL_ENDIAN_HPP
#define	RAMEN_UTIL_ENDIAN_HPP

#include<boost/cstdint.hpp>

namespace ramen
{

inline bool cpu_is_little_endian()
{
    #if defined(__PPC__) || defined(__ppc__) || defined(__POWERPC__) || defined(__powerpc__)
        return false;
    #else
        return true;
    #endif
}

inline boost::int16_t reverse_bytes( boost::int16_t x)
{
    return (( x & 255) << 8) | (( x >> 8) & 255 );
}

inline boost::uint16_t reverse_bytes( boost::uint16_t x)
{
    return (( x & 255) << 8) | (( x >> 8) & 255 );
}

inline boost::int32_t reverse_bytes( boost::int32_t x)
{
    return (( x & 255) << 24) | ((( x >> 8) & 255 ) << 16 ) |
	   ((( x >> 16) & 255 ) << 8 ) | (( x >> 24) & 255 );
}

inline boost::uint32_t reverse_bytes( boost::uint32_t x)
{
    return (( x & 255) << 24) | ((( x >> 8) & 255 ) << 16 ) |
	   ((( x >> 16) & 255 ) << 8 ) | (( x >> 24) & 255 );
}

inline float reverse_bytes( float x)
{
    BOOST_STATIC_ASSERT( sizeof( boost::uint32_t) == sizeof(float));

    union
    {
        boost::uint32_t i;
        float f;
    } xx;

    xx.f = x;
    xx.i = reverse_bytes( xx.i);
    return xx.f;
}

inline boost::int64_t reverse_bytes( boost::int64_t x)
{
	return  ((x & 255) << 56) |
                (((x >> 8) & 255) << 48) |
                (((x >> 16) & 255 ) << 40 ) |
                (((x >> 24) & 255 ) << 32 ) |
                (((x >> 32) & 255 ) << 24 ) |
                (((x >> 40) & 255 ) << 16 ) |
                (((x >> 48) & 255 ) << 8 ) |
                ((x >> 56) & 255 );
}

inline boost::uint64_t reverse_bytes( boost::uint64_t x)
{
	return  ((x & 255) << 56) |
                (((x >> 8) & 255) << 48) |
                (((x >> 16) & 255 ) << 40 ) |
                (((x >> 24) & 255 ) << 32 ) |
                (((x >> 32) & 255 ) << 24 ) |
                (((x >> 40) & 255 ) << 16 ) |
                (((x >> 48) & 255 ) << 8 ) |
                ((x >> 56) & 255 );
}

template<class T>
T as_big_endian( T x)
{
    if( cpu_is_little_endian())
        return reverse_bytes( x);
    else
        return x;
}

template<class T>
T as_little_endian( T x)
{
    if( !cpu_is_little_endian())
        return reverse_bytes( x);
    else
        return x;
}

} // namespace

#endif
