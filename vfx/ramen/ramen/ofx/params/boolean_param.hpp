// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_BOOL_PARAM_HPP
#define RAMEN_OFX_BOOL_PARAM_HPP

#include"ofxCore.h"
#include"ofxParam.h"

#include"ofxhPropertySuite.h"
#include"ofxhParam.h"

namespace ramen
{
namespace ofx
{

class image_effect_t;

class boolean_param_t : public OFX::Host::Param::BooleanInstance
{
public:

    boolean_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor, OFX::Host::Param::SetInstance* instance = 0);

    OfxStatus copy( const Instance &instance, OfxTime offset);
    OfxStatus copy( const Instance &instance, OfxTime offset, OfxRangeD range);

    OfxStatus get(bool& v);
    OfxStatus get(OfxTime time, bool& v);
    OfxStatus set(bool v);
    OfxStatus set(OfxTime time, bool v);

private:

    image_effect_t *effect_;
};

} // namespace
} // namespace

#endif
