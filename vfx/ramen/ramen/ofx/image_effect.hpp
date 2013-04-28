// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_IMAGE_EFFECT_HPP
#define RAMEN_OFX_IMAGE_EFFECT_HPP

#include"ofxCore.h"

#include"ofxhImageEffect.h"

namespace ramen
{

class ofx_node_t;

namespace ofx
{

class image_effect_t : public OFX::Host::ImageEffect::Instance
{
public:

    image_effect_t( OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                     OFX::Host::ImageEffect::Descriptor& desc,
                     const std::string& context, ofx_node_t *node);

    ~image_effect_t();

    const ofx_node_t *node() const  { return node_;}
    ofx_node_t *node()		    { return node_;}
    
    void set_node( ofx_node_t *node);

    virtual const std::string &getDefaultOutputFielding() const;

    /// make a clip
    OFX::Host::ImageEffect::ClipInstance* newClipInstance( OFX::Host::ImageEffect::Instance* plugin,
                                                          OFX::Host::ImageEffect::ClipDescriptor* descriptor,
                                                          int index);

    virtual OfxStatus vmessage( const char* type, const char* id, const char* format, va_list args);

    virtual void getProjectSize( double& xSize, double& ySize) const;
    virtual void getProjectOffset( double& xOffset, double& yOffset) const;
    virtual void getProjectExtent( double& xSize, double& ySize) const;

    virtual double getProjectPixelAspectRatio() const;
    virtual double getEffectDuration() const;

    virtual double getFrameRate() const;

    virtual double getFrameRecursive() const;

    virtual void getRenderScaleRecursive( double &x, double &y) const;

    virtual OFX::Host::Param::Instance* newParam( const std::string& name, OFX::Host::Param::Descriptor& Descriptor);

    virtual OfxStatus editBegin( const std::string& name);
    virtual OfxStatus editEnd();

    virtual void progressStart( const std::string &message);
    virtual void progressEnd();
    virtual bool progressUpdate( double t);

    virtual double timeLineGetTime();
    virtual void timeLineGotoTime( double t);
    virtual void timeLineGetBounds( double &t1, double &t2);

    virtual OfxStatus beginInstanceChangedAction( const std::string& why);
    virtual OfxStatus paramInstanceChangedAction( const std::string& paramName, const std::string& why, OfxTime time, OfxPointD renderScale);
    virtual OfxStatus clipInstanceChangedAction( const std::string& clipName, const std::string& why, OfxTime time, OfxPointD renderScale);
    virtual OfxStatus endInstanceChangedAction( const std::string& why);

    virtual void paramChangedByPlugin( OFX::Host::Param::Instance *param);

    virtual bool getClipPreferences();

private:

    // disabled
    image_effect_t( const image_effect_t& other);
    void operator=( const image_effect_t& other);

    ofx_node_t *node_;
};

} // namespace
} // namespace

#endif
