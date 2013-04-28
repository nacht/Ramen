// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_BERNSTEIN_HPP
#define	RAMEN_BEZIER_BERNSTEIN_HPP

#include<boost/array.hpp>

namespace ramen
{
namespace bezier
{

template<class T, int Degree>
T bernstein( int i, T u)
{
    T tmp[Degree + 1];

    for( int j = 0; j <= Degree; ++j)
	tmp[j] = T(0);

    tmp[Degree - i] = T(1);
    T u1 = T(1) - u;

    for( int k = 1; k <= Degree; ++k)
    {
	for( int j = Degree; j >= k; --j)
	    tmp[j] = u1 * tmp[j] + u * tmp[j-1];
    }

    return tmp[Degree];
}

template<class T, int Degree>
void all_bernstein( T u, boost::array<T, Degree+1>& B)
{
    B[0] = T(1);
    T u1 = T(1) - u;

    for( int j = 1; j <= Degree; ++j)
    {
        T saved = T(0);

        for( int k = 0; k < j; ++k)
        {
            T temp = B[k];
            B[k] = saved + u1 * temp;
            saved = u * temp;
        }

        B[j] = saved;
    }
}

} // namespace
} // namespace

#endif
