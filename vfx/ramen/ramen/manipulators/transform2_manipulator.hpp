// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TRANSFORM2_MANIPULATOR_HPP
#define RAMEN_TRANSFORM2_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{

class transform2_param_t;

class transform2_manipulator_t : public manipulator_t
{
public:

    transform2_manipulator_t( transform2_param_t *xform);

    virtual void draw_overlay( ui::image_view_t& view) const;

private:

    transform2_param_t *xform_;
};

} // namespace

#endif
