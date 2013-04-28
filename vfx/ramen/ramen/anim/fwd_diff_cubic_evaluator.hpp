// Copyright (c) 2010 Esteban floatovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_FWD_DIFF_CUBIC_EVALUATOR_HPP
#define	RAMEN_ANIM_FWD_DIFF_CUBIC_EVALUATOR_HPP

#include<boost/array.hpp>

namespace ramen
{
namespace anim
{

class fwd_diff_cubic_evaluator_t
{
public:

    fwd_diff_cubic_evaluator_t( const boost::array<float,4>& p, float step_size)
    {
        float s = step_size;
        float s2 = s * s;
        float s3 = s2 * s;

        value_ = p[3];
        diff1_ = p[0] * s3 + p[1] * s2 + p[2] * s;
        diff2_ = float(6) * p[0] * s3 + float(2) * p[1] * s2;
        diff3_ = float(6) * p[0] * s3;
    }

    float operator()() const { return value_;}

    fwd_diff_cubic_evaluator_t& operator++()
    {
        value_ += diff1_;
        diff1_ += diff2_;
        diff2_ += diff3_;
        return *this;
    }

    fwd_diff_cubic_evaluator_t operator++( int)
    {
        fwd_diff_cubic_evaluator_t tmp( *this);
        ++( *this);
        return tmp;
    }

private:

    float value_;
    float diff1_, diff2_, diff3_;
};

} // anim
} // ramen

#endif
