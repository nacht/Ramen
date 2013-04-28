// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_CLIP_HPP
#define RAMEN_OFX_CLIP_HPP

#include<ramen/ofx/image.hpp>

namespace ramen
{

class ofx_node_t;

namespace ofx
{

class clip_t : public OFX::Host::ImageEffect::ClipInstance
{
public:

    clip_t( OFX::Host::ImageEffect::Instance* effectInstance, OFX::Host::ImageEffect::ClipDescriptor& desc);

    const ofx_node_t *node() const  { return node_;}
    ofx_node_t *node()		    { return node_;}

    int port() const { return port_;}

    const std::string &getUnmappedBitDepth() const;
    virtual const std::string &getUnmappedComponents() const;
    virtual const std::string &getPremult() const;
    virtual double getAspectRatio() const;
    virtual double getFrameRate() const;
    virtual void getFrameRange(double &startFrame, double &endFrame) const ;
    virtual const std::string &getFieldOrder() const;
    virtual bool getConnected() const;
    virtual double getUnmappedFrameRate() const;
    virtual void getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const;
    virtual bool getContinuousSamples() const;
    virtual OfxRectD getRegionOfDefinition(OfxTime time) const;

    virtual OFX::Host::ImageEffect::Image* getImage( OfxTime time, OfxRectD *optionalBounds);

private:

    OFX::Host::ImageEffect::Image* get_input_image( OfxTime time, OfxRectD *optionalBounds);
    OFX::Host::ImageEffect::Image* get_output_image( OfxTime time, OfxRectD *optionalBounds);

    ofx_node_t *node_;
    int port_;
};

} // namespace
} // namespace

#endif
