// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_MATRIX_ALGO_HPP
#define RAMEN_IMATHEXT_MATRIX_ALGO_HPP

#include<boost/optional.hpp>
#include<boost/array.hpp>

#include<OpenEXR/ImathMatrix.h>

#include<Eigen/Core>
#include<Eigen/LU>

namespace ramen
{

template<class T>
bool isAffine( const Imath::Matrix33<T>& m)
{
    return m[0][2] == 0 && m[1][2] == 0 && m[2][2] == 1;
}

template<class T>
boost::optional<Imath::Matrix33<T> > quadToQuadMatrix( const boost::array<Imath::Vec2<T> ,4>& src, const boost::array<Imath::Vec2<T> ,4>& dst)
{
    Eigen::Matrix<T, 8, 8> A( Eigen::Matrix<T,8,8>::Zero());
    Eigen::Matrix<T, 8, 1> b, x;

    for( int i = 0; i < 4; ++i)
    {
	A( i, 0) = src[i].x;
	A( i, 1) = src[i].y;
	A( i, 2) = 1;

	A( i, 6) = -src[i].x*dst[i].x;
	A( i, 7) = -src[i].y*dst[i].x;

	A( 4 + i, 3) = src[i].x;
	A( 4 + i, 4) = src[i].y;
	A( 4 + i, 5) = 1;

	A( 4 + i, 6) = -src[i].x*dst[i].y;
	A( 4 + i, 7) = -src[i].y*dst[i].y;

        b[i]     = dst[i].x;
        b[4 + i] = dst[i].y;
    }

    Eigen::LU<Eigen::Matrix<T,8,8> > lu_decomp( A);

    if( lu_decomp.solve( b, &x))
    {
        return Imath::Matrix33<T>(  x[0], x[3], x[6],
                                    x[1], x[4], x[7],
                                    x[2], x[5], 1);
    }
    else
        return boost::optional<Imath::Matrix33<T> >();
}

// color manipulation matrices

Imath::M44f hueRotationMatrix( float angle);
Imath::M44f saturationMatrix( float sat);
Imath::M44f gainMatrix( const Imath::V3f& g);
Imath::M44f gainMatrix( float g);

} // namespace

#endif
