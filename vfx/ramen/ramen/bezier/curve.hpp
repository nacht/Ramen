// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_CURVE_HPP
#define	RAMEN_BEZIER_CURVE_HPP

#include<cmath>

#include<boost/config.hpp>
#include<boost/static_assert.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/bezier/bernstein.hpp>

namespace ramen
{
namespace bezier
{

// P is Imath::Vec 2, 3 or a similar class
template<class P, int Degree = 3>
class curve_t
{
    BOOST_STATIC_ASSERT( Degree >= 1);

public:

    BOOST_STATIC_CONSTANT( int, degree = Degree);
    BOOST_STATIC_CONSTANT( int, order  = Degree + 1);

    typedef P                           point_type;
    typedef P                           vector_type;
    typedef typename P::BaseType        param_type;
    typedef typename P::BaseType        coord_type;
    typedef typename Imath::Box<P>      box_type;

    curve_t() {}

    curve_t( const P& q0, const P& q1)
    {
        BOOST_STATIC_ASSERT( degree == 1);
        p[0] = q0;
	p[1] = q1;
    }

    curve_t( const P& q0, const P& q1, const P& q2)
    {
        BOOST_STATIC_ASSERT( degree == 2);
        p[0] = q0;
	p[1] = q1;
	p[2] = q2;
    }

    curve_t( const P& q0, const P& q1, const P& q2, const P& q3)
    {
        BOOST_STATIC_ASSERT( degree == 3);
        p[0] = q0;
	p[1] = q1;
	p[2] = q2;
	p[3] = q3;
    }

    P operator()( param_type t) const
    {
        boost::array<param_type, order> B;
        all_bernstein<param_type, degree>( t, B);

        point_type q;

        for( unsigned int j = 0; j < point_type::dimensions(); ++j)
        {
            q[j] = 0;

            for( int i = 0; i < order; ++i)
                q[j] += B[i] * p[i][j];
        }

        return q;
    }

    box_type bounding_box() const
    {
        box_type b;

        for( int i = 0; i < order; ++i)
            b.extendBy( p[i]);

        return b;
    }

    curve_t<P, Degree-1> derivative_curve() const
    {
        BOOST_STATIC_ASSERT( degree > 1);

        curve_t<P, Degree-1> result;

	for( int i = 0; i < Degree; ++i)
	{
            point_type q;

            for( unsigned int j = 0; j < point_type::dimensions(); ++j)
                q[j] = p[i+1][j] - p[i][j];

	    result.p[i] = q;
	}

	return result;
    }

    vector_type start_derivative1() const
    {
        BOOST_STATIC_ASSERT( degree >= 1);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( p[1][i] - p[0][i]);

        return result;
    }

    vector_type end_derivative1() const
    {
        BOOST_STATIC_ASSERT( degree >= 1);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( p[Degree][i] - p[Degree-1][i]);

        return result;
    }

    vector_type start_derivative2() const
    {
        BOOST_STATIC_ASSERT( degree >= 2);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( Degree - 1) * ( p[2][i] - p[1][i] - p[1][i] + p[0][i]);

        return result;
    }

    vector_type end_derivative2() const
    {
        BOOST_STATIC_ASSERT( degree >= 2);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( Degree - 1) * ( p[Degree][i] - p[Degree - 1][i] - p[Degree - 1][i] + p[Degree - 2][i]);

        return result;
    }

    coord_type start_curvature() const
    {
        vector_type d1( start_derivative1());
        vector_type d2( start_derivative2());
        coord_type k = ( d1.x * d2.y) - ( d1.y * d2.x);
        return k / ( std::pow( (double) d1.length2(), 3.0 / 2.0));
    }

    coord_type end_curvature() const
    {
        vector_type d1( end_derivative1());
        vector_type d2( end_derivative2());
        coord_type k = ( d1.x * d2.y) - ( d1.y * d2.x);
        return k / ( std::pow( (double) d1.length2(), 3.0 / 2.0));
    }

    P p[Degree+1];
};

} // namespace
} // namespace

#endif
