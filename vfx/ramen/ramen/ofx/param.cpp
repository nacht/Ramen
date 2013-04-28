// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/param.hpp>

#include<ramen/params/composite_param.hpp>
#include<ramen/params/group_param.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/button_param.hpp>
#include<ramen/params/color_param.hpp>

namespace ramen
{
namespace ofx
{

bool log_param_info = true;

param_factory_t::param_factory_t() {}

std::auto_ptr<param_t> param_factory_t::create_param( OFX::Host::Param::Descriptor *d)
{
    std::auto_ptr<param_t> p;

    if( d->getType() == kOfxParamTypePage)
    {
	p.reset( new composite_param_t( d->getShortLabel()));
	p->set_id( d->getName());
	return p;
    }

    if( d->getType() == kOfxParamTypeGroup)
	p.reset( new group_param_t( d->getShortLabel()));

    if( d->getType() == kOfxParamTypeDouble)
	p.reset( create_double_param( d));

    if( d->getType() == kOfxParamTypeDouble2D)
	p.reset( create_double2d_param( d));

    if( d->getType() == kOfxParamTypeInteger)
	p.reset( create_int_param( d));

    if( d->getType() == kOfxParamTypeChoice)
	p.reset( create_choice_param( d));

    if( d->getType() == kOfxParamTypeBoolean)
	p.reset( create_bool_param( d));

    if( d->getType() == kOfxParamTypePushButton)
	p.reset( create_button_param( d));

    if( d->getType() == kOfxParamTypeRGB)
	p.reset( create_rgb_param( d));

    if( d->getType() == kOfxParamTypeRGBA)
	p.reset( create_rgba_param( d));

    if( !p.get())
    {
	// We should throw an exception here!
	return p;
    }

    // set common param options
    p->set_id( d->getName());
    p->set_enabled( d->getEnabled());
    p->set_secret( d->getSecret());
    p->set_can_undo( d->getCanUndo());
    return p;
}

param_t *param_factory_t::create_double_param( OFX::Host::Param::Descriptor *d)
{
    float_param_t *p = new float_param_t( d->getShortLabel());
    p->set_default_value( d->getProperties().getDoubleProperty( kOfxParamPropDefault, 0));
    set_numeric_type( d, p);
    p->set_step( step( d));
    set_double_range( d, p);
    return p;
}

param_t *param_factory_t::create_double2d_param( OFX::Host::Param::Descriptor *d)
{
    float2_param_t *p = new float2_param_t( d->getShortLabel());
    Imath::V2f x( d->getProperties().getDoubleProperty( kOfxParamPropDefault, 0),
		   d->getProperties().getDoubleProperty( kOfxParamPropDefault, 1));

    p->set_default_value( x);
    set_double_range( d, p);
    set_numeric_type( d, p);
    p->set_step( step( d));
    return p;
}

param_t *param_factory_t::create_int_param( OFX::Host::Param::Descriptor *d)
{
    float_param_t *p = new float_param_t( d->getShortLabel());
    p->set_default_value( d->getProperties().getIntProperty( kOfxParamPropDefault, 0));
    set_int_range( d, p);
    p->set_step( 1);
    p->set_round_to_int( true);
    return p;
}

param_t *param_factory_t::create_bool_param( OFX::Host::Param::Descriptor *d)
{
    bool_param_t *p = new bool_param_t( d->getShortLabel());
    p->set_default_value( d->getProperties().getIntProperty( kOfxParamPropDefault, 0));
    return p;
}

param_t *param_factory_t::create_choice_param( OFX::Host::Param::Descriptor *d)
{
    popup_param_t *p = new popup_param_t( d->getShortLabel());
    p->set_default_value( d->getProperties().getIntProperty( kOfxParamPropDefault, 0));

    std::string key( kOfxParamPropChoiceOption);
    int num_items = d->getProperties().getDimension( key);

    for( int i = 0; i < num_items; ++i)
	p->menu_items() += d->getProperties().getStringProperty( key, i);

    return p;
}

param_t *param_factory_t::create_button_param( OFX::Host::Param::Descriptor *d)
{
    button_param_t *p = new button_param_t( d->getShortLabel());
    return p;
}

param_t *param_factory_t::create_rgb_param( OFX::Host::Param::Descriptor *d)
{
    color_param_t *p = new color_param_t( d->getShortLabel());
    p->set_is_rgba( false);

    Imath::Color4f x( d->getProperties().getDoubleProperty( kOfxParamPropDefault, 0),
			d->getProperties().getDoubleProperty( kOfxParamPropDefault, 1),
			d->getProperties().getDoubleProperty( kOfxParamPropDefault, 2), 1);

    p->set_default_value( x);
    return p;
}

param_t *param_factory_t::create_rgba_param( OFX::Host::Param::Descriptor *d)
{
    color_param_t *p = new color_param_t( d->getShortLabel());

    Imath::Color4f x( d->getProperties().getDoubleProperty( kOfxParamPropDefault, 0),
			d->getProperties().getDoubleProperty( kOfxParamPropDefault, 1),
			d->getProperties().getDoubleProperty( kOfxParamPropDefault, 2),
		      d->getProperties().getDoubleProperty( kOfxParamPropDefault, 3));

    p->set_default_value( x);
    return p;
}

float param_factory_t::step( OFX::Host::Param::Descriptor *d)
{
    if( d->getDoubleType() == kOfxParamDoubleTypeNormalisedX ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedXAbsolute ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedY ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedYAbsolute ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedXY ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedXYAbsolute)
    {
	return 1.0f;
    }

    float step = d->getProperties().getDoubleProperty( kOfxParamPropIncrement, 0);

    if( step == 0.0f)
    {
	float lo = d->getProperties().getIntProperty( kOfxParamPropMin, 0);
	float hi = d->getProperties().getIntProperty( kOfxParamPropMax, 0);

	step = ( hi - lo) / 50.0f;

	// fallback
	if( step == 0.0f)
	    step = 0.05f;
    }

    return step;
}

void param_factory_t::set_numeric_type( OFX::Host::Param::Descriptor *d, param_t *p)
{
    numeric_param_t *q = dynamic_cast<numeric_param_t*>( p);
    assert( q);

    if( d->getDoubleType() == kOfxParamDoubleTypePlain ||
	    d->getDoubleType() == kOfxParamDoubleTypeAngle ||
	    d->getDoubleType() == kOfxParamDoubleTypeScale ||
	    d->getDoubleType() == kOfxParamDoubleTypeTime ||
	    d->getDoubleType() == kOfxParamDoubleTypeAbsoluteTime)
    {
	return;
    }

    if( d->getDoubleType() == kOfxParamDoubleTypeNormalisedX ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedXAbsolute)
    {
	q->set_numeric_type( numeric_param_t::relative_x);
	return;
    }

    if( d->getDoubleType() == kOfxParamDoubleTypeNormalisedY ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedYAbsolute)
    {
	q->set_numeric_type( numeric_param_t::relative_y);
	return;
    }

    if( d->getDoubleType() == kOfxParamDoubleTypeNormalisedXY ||
	    d->getDoubleType() == kOfxParamDoubleTypeNormalisedXYAbsolute)
    {
	q->set_numeric_type( numeric_param_t::relative_xy);
	return;
    }
}

void param_factory_t::set_double_range( OFX::Host::Param::Descriptor *d, param_t *p)
{
    animated_param_t *q = dynamic_cast<animated_param_t*>( p);
    assert( q);

    double low  = d->getProperties().getDoubleProperty( kOfxParamPropMin, 0);
    double high = d->getProperties().getDoubleProperty( kOfxParamPropMax, 0);

    if( low > (double) -std::numeric_limits<float>::max())
	q->set_min( low);

    if( high < (double) std::numeric_limits<float>::max())
	q->set_max( high);
}

void param_factory_t::set_int_range( OFX::Host::Param::Descriptor *d, param_t *p)
{
    animated_param_t *q = dynamic_cast<animated_param_t*>( p);
    assert( q);

    int low  = d->getProperties().getIntProperty( kOfxParamPropMin, 0);
    int high = d->getProperties().getIntProperty( kOfxParamPropMax, 0);

    if( low != -std::numeric_limits<int>::max())
	q->set_min( low);

    if( high != std::numeric_limits<int>::max())
	q->set_max( high);
}

} // ofx
} // ramen
