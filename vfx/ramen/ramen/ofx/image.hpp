// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_IMAGE_HPP
#define RAMEN_OFX_IMAGE_HPP

#include"ofxCore.h"

#include"ofxhPropertySuite.h"
#include"ofxhClip.h"

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/image/image_buffer.hpp>

namespace ramen
{
namespace ofx
{

class input_image_t : public OFX::Host::ImageEffect::Image
{
public:

    input_image_t( OFX::Host::ImageEffect::ClipInstance& instance, image::image_buffer_t image,
		    double renderScale, void* data, const OfxRectI &bounds, const OfxRectI &rod, int rowBytes,
			std::string uniqueIdentifier);

    ~input_image_t();

private:

    image::image_buffer_t pixels_;
};

class output_image_t : public OFX::Host::ImageEffect::Image
{
public:

    output_image_t( OFX::Host::ImageEffect::ClipInstance& instance, double renderScale, void* data,
			const OfxRectI &bounds, const OfxRectI &rod, int rowBytes, std::string uniqueIdentifier);

    ~output_image_t();

private:

    node_t *node_;
};

} // namespace
} // namespace

#endif
