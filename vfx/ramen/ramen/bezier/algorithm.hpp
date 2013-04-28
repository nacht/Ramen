// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_ALGORITHM_HPP
#define	RAMEN_BEZIER_ALGORITHM_HPP

#include<ramen/bezier/curve.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/ggems/nearestpoint.h>

namespace ramen
{
namespace bezier
{

template<class P>
void split_curve( const curve_t<P,3>& c, double t, curve_t<P,3>& a, curve_t<P,3>& b)
{
    typedef typename curve_t<P,3>::point_type point_type;

    point_type p10, p11, p12, p20, p21, p30;

    for( unsigned int j = 0; j < point_type::dimensions(); ++j)
    {
        p10[j] = Imath::lerp( c.p[0][j], c.p[1][j], t);
        p11[j] = Imath::lerp( c.p[1][j], c.p[2][j], t);
        p12[j] = Imath::lerp( c.p[2][j], c.p[3][j], t);

        p20[j] = Imath::lerp( p10[j], p11[j], t);
        p21[j] = Imath::lerp( p11[j], p12[j], t);

        p30[j] = Imath::lerp( p20[j], p21[j], t);
    }

    a.p[0] = c.p[0];
    a.p[1] = p10;
    a.p[2] = p20;
    a.p[3] = p30;

    b.p[0] = p30;
    b.p[1] = p21;
    b.p[2] = p12;
    b.p[3] = c.p[3];
}

// Optimized for t = 0.5. It's a very common case.
template<class P>
void split_curve_midpoint( const curve_t<P,3>& c, curve_t<P,3>& a, curve_t<P,3>& b)
{
    typedef typename curve_t<P,3>::point_type   point_type;

    point_type p10, p11, p12, p20, p21, p30;

    double t = 0.5;

    for( unsigned int j = 0; j < point_type::dimensions(); ++j)
    {
        p10[j] = ( c.p[0][j] + c.p[1][j]) * t;
        p11[j] = ( c.p[1][j] + c.p[2][j]) * t;
        p12[j] = ( c.p[2][j] + c.p[3][j]) * t;

        p20[j] = ( p10[j] + p11[j]) * t;
        p21[j] = ( p11[j] + p12[j]) * t;

        p30[j] = ( p20[j] + p21[j]) * t;
    }

    a.p[0] = c.p[0];
    a.p[1] = p10;
    a.p[2] = p20;
    a.p[3] = p30;

    b.p[0] = p30;
    b.p[1] = p21;
    b.p[2] = p12;
    b.p[3] = c.p[3];
}

template<class P, class T>
P nearest_point_on_curve( const curve_t<P,3>& c, const P& p, T& out_t)
{
    typedef typename curve_t<P,3>::point_type point_type;

    assert( point_type::dimensions() == 2 && "nearest point on curve works only in 2d");

    Point2 bezier[4];
    Point2 ggp;
    double result_param;

    for( int i = 0; i < 4; ++i)
    {
        bezier[i].x = c.p[i].x;
        bezier[i].y = c.p[i].y;
    }

    ggp.x = p.x;
    ggp.y = p.y;

    Point2 result_point = NearestPointOnCurve( ggp, bezier, &result_param);

    out_t = result_param;
    return P( result_point.x, result_point.y);
}

} // namespace
} // namespace

#endif
