// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/pushbutton_param.hpp>

namespace ramen
{
namespace ofx
{

pushbutton_param_t::pushbutton_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::PushbuttonInstance( descriptor, instance)
{
}

} // namespace
} // namespace
