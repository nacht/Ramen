// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/rgba_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/color_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

rgba_color_param_t::rgba_color_param_t( const std::string& name,
				       OFX::Host::Param::Descriptor& descriptor,
				       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::RGBAInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus rgba_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    const rgba_color_param_t *other = dynamic_cast<const rgba_color_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    const rgba_color_param_t *other = dynamic_cast<const rgba_color_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::get( double& r, double& g, double& b, double& a)
{
    Imath::Color4f x( get_value<Imath::Color4f>( effect_->node()->param( getName())));
    r = x.r; g = x.g; b = x.b; a = x.a;
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::get( OfxTime time, double& r, double& g, double& b, double& a)
{
    Imath::Color4f x( get_value_at_time<Imath::Color4f>( effect_->node()->param( getName()), time));
    r = x.r; g = x.g; b = x.b; a = x.a;

    bool do_log = true;

   if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;

    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::set( double r, double g, double b, double a)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, r, g, b, a);
}

OfxStatus rgba_color_param_t::set( OfxTime time, double r, double g, double b, double a)
{
    effect_->node()->block_param_changed_signal( true);
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_time( Imath::Color4f( r, g, b, a), time);
    param.update_widgets();
    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::getNumKeys( unsigned int &nKeys) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    nKeys = param.get_num_keys();
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::getKeyTime( int nth, OfxTime& time) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    time = param.get_key_time( nth);
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    param.get_key_index( time, direction, index);

    if( index != -1)
	return kOfxStatOK;
    else
	return kOfxStatFailed;
}

OfxStatus rgba_color_param_t::deleteKey( OfxTime time)
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.delete_key( time);
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::deleteAllKeys()
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.delete_all_keys();
    return kOfxStatOK;
}

} // namespace
} // namespace
