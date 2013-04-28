// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/image.hpp>

#include"ofxImageEffect.h"

namespace ramen
{
namespace ofx
{

input_image_t::input_image_t( OFX::Host::ImageEffect::ClipInstance& instance,
		  image::image_buffer_t image,
              double renderScale,
              void* data,
              const OfxRectI &bounds,
              const OfxRectI &rod,
              int rowBytes,
              std::string uniqueIdentifier) : OFX::Host::ImageEffect::Image( instance, renderScale, renderScale,
											  data, bounds, rod, rowBytes,
											  std::string( kOfxImageFieldNone),
											  uniqueIdentifier)
{
    pixels_ = image;
}

input_image_t::~input_image_t()
{
}

output_image_t::output_image_t( OFX::Host::ImageEffect::ClipInstance& instance,
		    double renderScale, void* data,
              const OfxRectI &bounds, const OfxRectI &rod,
		 int rowBytes, std::string uniqueIdentifier) : OFX::Host::ImageEffect::Image( instance, renderScale, renderScale,
											  data, bounds, rod, rowBytes,
											  std::string( kOfxImageFieldNone),
											  uniqueIdentifier)
{}

output_image_t::~output_image_t()
{
}

} // namespace
} // namespace
