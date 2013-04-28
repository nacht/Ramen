// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_RGBA_PARAM_HPP
#define RAMEN_OFX_RGBA_PARAM_HPP

#include"ofxCore.h"
#include"ofxParam.h"

#include"ofxhPropertySuite.h"
#include"ofxhParam.h"

namespace ramen
{

namespace ofx
{

class image_effect_t;

class rgba_color_param_t : public OFX::Host::Param::RGBAInstance
{
public:

    rgba_color_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor, OFX::Host::Param::SetInstance* instance = 0);

    OfxStatus copy( const Instance &instance, OfxTime offset);
    OfxStatus copy( const Instance &instance, OfxTime offset, OfxRangeD range);

    OfxStatus get( double& r, double& g, double& b, double& a);
    OfxStatus get( OfxTime time, double& r, double& g, double& b, double& a);
    OfxStatus set( double r, double g, double b, double a);
    OfxStatus set( OfxTime time, double r, double g, double b, double a);

    OfxStatus getNumKeys( unsigned int &nKeys) const;
    OfxStatus getKeyTime( int nth, OfxTime& time) const;
    OfxStatus getKeyIndex( OfxTime time, int direction, int & index) const;
    OfxStatus deleteKey( OfxTime time);
    OfxStatus deleteAllKeys();

private:

    image_effect_t *effect_;
};

} // namespace
} // namespace

#endif
