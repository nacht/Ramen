// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/image_effect.hpp>

#include<stdio.h>

#include<QApplication>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/clip.hpp>
#include<ramen/ofx/param.hpp>
#include<ramen/ofx/host.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/app/application.hpp>
#include<ramen/app/preferences.hpp>
#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace ofx
{

image_effect_t::image_effect_t(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
				 OFX::Host::ImageEffect::Descriptor& desc, const std::string& context,
			       ofx_node_t *node)
				 : OFX::Host::ImageEffect::Instance(plugin,desc,context,false), node_( node)
{
}

image_effect_t::~image_effect_t() { endInstanceEditAction();}

void image_effect_t::set_node( ofx_node_t *node) { node_ = node;}

OFX::Host::ImageEffect::ClipInstance* image_effect_t::newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
								      OFX::Host::ImageEffect::ClipDescriptor* descriptor,
								      int index)
{
    return new clip_t( this, *descriptor);
}

const std::string &image_effect_t::getDefaultOutputFielding() const
{
    static const std::string v( kOfxImageFieldNone);
    return v;
}

OfxStatus image_effect_t::vmessage(const char* type, const char* id, const char* format, va_list args)
{
    return ofx::host_t::Instance().vmessage( type, id, format, args);
}

void image_effect_t::getProjectSize( double& xSize, double& ySize) const
{
    assert( node());

    if( !node()->composition())
    {
	xSize = preferences_t::Instance().default_format().width;
	ySize = preferences_t::Instance().default_format().height;
    }
    else
    {
        if( node()->num_inputs() > 0 && node()->input())
        {
            const node_t *n = node()->input();
            Imath::Box2i proj_size = n->domain();

            xSize = ( proj_size.size().x + 1);
            ySize = ( proj_size.size().y + 1);
        }
        else
        {
            xSize = node()->composition()->default_format().width;
            ySize = node()->composition()->default_format().height;
        }
    }
}

void image_effect_t::getProjectOffset(double& xOffset, double& yOffset) const { xOffset = yOffset = 0;}

void image_effect_t::getProjectExtent(double& xSize, double& ySize) const { getProjectSize( xSize, ySize);}

double image_effect_t::getProjectPixelAspectRatio() const { return 1.0;}

double image_effect_t::getEffectDuration() const
{
    assert( node());

    if( node()->composition())
	return node()->composition()->end_frame() - node()->composition()->start_frame();
    else
	return 100;
}

double image_effect_t::getFrameRate() const
{
    assert( node());

    if( node()->composition())
	return node()->composition()->frame_rate();
    else
	return preferences_t::Instance().frame_rate();
}

double image_effect_t::getFrameRecursive() const
{
    if( node()->composition())
	return node()->composition()->frame();
    else
	return 1;
}

void image_effect_t::getRenderScaleRecursive(double &x, double &y) const
{
    assert( node());

    if( node()->composition())
	x = y = 1.0 / node()->composition()->subsample();
    else
	x = y = 1.0;
}

// make a parameter instance
OFX::Host::Param::Instance* image_effect_t::newParam( const std::string& name, OFX::Host::Param::Descriptor& descriptor)
{
    if( descriptor.getType()==kOfxParamTypeInteger)
      return new integer_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeDouble)
      return new double_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeBoolean)
      return new boolean_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeChoice)
      return new choice_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeRGBA)
      return new rgba_color_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeRGB)
      return new rgb_color_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeDouble2D)
      return new double2d_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypePushButton)
      return new pushbutton_param_t( name, descriptor, this);

    if( descriptor.getType()==kOfxParamTypeGroup)
      return new OFX::Host::Param::GroupInstance( descriptor, this);

    if( descriptor.getType()==kOfxParamTypePage)
      return new OFX::Host::Param::PageInstance( descriptor, this);

    return 0;
}

OfxStatus image_effect_t::editBegin( const std::string& name)
{
    node()->param_set().begin_edit();
    return kOfxStatOK;
}

OfxStatus image_effect_t::editEnd()
{
    node()->param_set().end_edit();
    return kOfxStatOK;
}

void image_effect_t::progressStart( const std::string &message)
{
}

void image_effect_t::progressEnd()
{
}

bool image_effect_t::progressUpdate( double t)
{
    return true;
}

double image_effect_t::timeLineGetTime()
{
    assert( node());

    double time = 1.0;

    if( node()->composition())
	time = node()->composition()->frame();

    return time;
}

void image_effect_t::timeLineGotoTime(double t)
{
    assert( node());
    assert( node()->composition());

    if( application_t::Instance().run_command_line())
	node()->composition()->set_frame( t);
    else
    {
	ui::user_interface_t::Instance().set_frame( t);
	qApp->processEvents();
    }
}

void image_effect_t::timeLineGetBounds(double &t1, double &t2)
{
    assert( node());

    if( node()->composition())
    {
	t1 = node()->composition()->start_frame();
	t2 = node()->composition()->end_frame();
    }
    else
    {
	t1 = 1;
	t2 = 100;
    }
}

OfxStatus image_effect_t::beginInstanceChangedAction( const std::string& why)
{
    return OFX::Host::ImageEffect::Instance::beginInstanceChangedAction( why);
}

OfxStatus image_effect_t::paramInstanceChangedAction( const std::string& paramName, const std::string& why, OfxTime time, OfxPointD renderScale)
{
    return OFX::Host::ImageEffect::Instance::paramInstanceChangedAction( paramName, why, time, renderScale);
}

OfxStatus image_effect_t::clipInstanceChangedAction( const std::string& clipName, const std::string& why, OfxTime time, OfxPointD renderScale)
{
    return OFX::Host::ImageEffect::Instance::clipInstanceChangedAction( clipName, why, time, renderScale);
}

OfxStatus image_effect_t::endInstanceChangedAction( const std::string& why)
{
    return OFX::Host::ImageEffect::Instance::endInstanceChangedAction( why);
}

void image_effect_t::paramChangedByPlugin( OFX::Host::Param::Instance *param)
{
    OFX::Host::ImageEffect::Instance::paramChangedByPlugin( param);
}

bool image_effect_t::getClipPreferences()
{
    bool result = OFX::Host::ImageEffect::Instance::getClipPreferences();
    return result;
}

} // namespace
} // namespace
