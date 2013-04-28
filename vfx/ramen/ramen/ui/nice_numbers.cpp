// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/nice_numbers.hpp>

#include<algorithm>
#include<cmath>

namespace ramen
{
namespace ui
{

nice_numbers_t::nice_numbers_t( double imin, double imax, int num_ticks) : nticks_(num_ticks)
{
    range_ = nice_num( imax - imin, false);
    d_ = nice_num( range_ / ( nticks_ - 1), true);
    graphmin_ = std::floor( imin / d_) * d_;
    graphmax_ = std::ceil( imax / d_) * d_;
    nfrac_ = std::max( -std::floor( std::log10( d_)), 0.0);
}

double nice_numbers_t::nice_num( double x, bool round) const
{
    int expv = std::floor( std::log10( x));
    double f = x / std::pow( 10.0, expv);    /* between 1 and 10 */
    double nf;				    /* nice, rounded fraction */

    if (round)
    {
	if (f<1.5)
	    nf = 1.0;
	else
	    if( f<3.0)
		nf = 2.0;
	    else
		if(f<7.0)
		    nf = 5.0;
		else
		    nf = 10.0;
    }
    else
    {
	if (f<=1.0)
	    nf = 1.0;
	else
	    if(f<=2.0)
		nf = 2.;
	    else
		if (f<=5.)
		    nf = 5.;
		else
		    nf = 10.;
    }

    return nf * std::pow(10.0, expv);
}

} // namespace
} // namespace
