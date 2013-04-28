// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef HALF_HALF_LUT_HPP
#define	HALF_HALF_LUT_HPP

#include<boost/scoped_array.hpp>

#include<OpenEXR/half.h>

class half_lut_t
{
public:

    half_lut_t() {}

    template<class Fun>
    half_lut_t( Fun f, half domain_min = -HALF_MAX,
                       half domain_max =  HALF_MAX,
                       half default_value = 0,
                       half pos_inf_value = 0,
                       half neg_inf_value = 0,
                       half nan_value = 0)
    {
        init( f, domain_min, domain_max, default_value, pos_inf_value,
                neg_inf_value, nan_value);
    }

    template<class Fun>
    void init( Fun f, half domain_min = -HALF_MAX,
                       half domain_max =  HALF_MAX,
                       half default_value = 0,
                       half pos_inf_value = 0,
                       half neg_inf_value = 0,
                       half nan_value = 0)
    {
	if( !lut_)
	    lut_.reset( new half[1<<16]);

	for( int i = 0; i < (1 << 16); ++i)
	{
	    half x;
	    x.setBits( i);

            if( x.isNan())
                lut_[i] = nan_value;
            else if( x.isInfinity())
                lut_[i] = x.isNegative()? neg_inf_value: pos_inf_value;
            else if( x < domain_min || x > domain_max)
                lut_[i] = default_value;
            else
                lut_[i] = f(x);
	}
    }

    half operator()( half x) const
    {
        assert( lut_.get() && "Trying to use an empty half lut");
        return lut_[x.bits()];
    }

    float operator()( float x) const
    {
        assert( lut_.get() && "Trying to use an empty half lut");
        half h = x;
        return lut_[h.bits()];
    }

    void swap( half_lut_t& other) { lut_.swap( other.lut_);}

private:

    boost::scoped_array<half> lut_;
};

#endif
