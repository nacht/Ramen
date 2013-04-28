// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_PARAM_HPP
#define RAMEN_OFX_PARAM_HPP

#include<memory>

#include<ramen/params/param.hpp>

#include<ramen/ofx/params/boolean_param.hpp>
#include<ramen/ofx/params/choice_param.hpp>
#include<ramen/ofx/params/double2d_param.hpp>
#include<ramen/ofx/params/double_param.hpp>
#include<ramen/ofx/params/integer_param.hpp>
#include<ramen/ofx/params/pushbutton_param.hpp>
#include<ramen/ofx/params/rgb_param.hpp>
#include<ramen/ofx/params/rgba_param.hpp>

namespace ramen
{
namespace ofx
{

class param_factory_t
{
public:

    static std::auto_ptr<param_t> create_param( OFX::Host::Param::Descriptor *d);

private:

    param_factory_t();

    static param_t *create_double_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_double2d_param( OFX::Host::Param::Descriptor *d);

    static param_t *create_int_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_bool_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_choice_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_button_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_rgb_param( OFX::Host::Param::Descriptor *d);
    static param_t *create_rgba_param( OFX::Host::Param::Descriptor *d);

    static float step( OFX::Host::Param::Descriptor *d);
    static void set_numeric_type( OFX::Host::Param::Descriptor *d, param_t *p);
    static void set_double_range( OFX::Host::Param::Descriptor *d, param_t *p);
    static void set_int_range( OFX::Host::Param::Descriptor *d, param_t *p);
};

} // ofx
} // ramen

#endif
