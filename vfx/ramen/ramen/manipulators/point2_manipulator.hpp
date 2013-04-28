// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_POINT2_MANIPULATOR_HPP
#define RAMEN_POINT2_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{

class float2_param_t;

class point2_manipulator_t : public manipulator_t
{
public:

    point2_manipulator_t( float2_param_t *point);

    virtual void draw_overlay( ui::image_view_t& view) const;

private:

    float2_param_t *point_;
};

} // namespace

#endif
