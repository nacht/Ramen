// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/rgb_param.hpp>

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

rgb_color_param_t::rgb_color_param_t( const std::string& name,
				     OFX::Host::Param::Descriptor& descriptor,
				     OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::RGBInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus rgb_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    const rgb_color_param_t *other = dynamic_cast<const rgb_color_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    const rgb_color_param_t *other = dynamic_cast<const rgb_color_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::get( double& r, double& g, double& b)
{
    Imath::Color4f x( get_value<Imath::Color4f>( effect_->node()->param( getName())));
    r = x.r; g = x.g; b = x.b;
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::get( OfxTime time, double& r, double& g, double& b)
{
    Imath::Color4f x( get_value_at_time<Imath::Color4f>( effect_->node()->param( getName()), time));
    r = x.r; g = x.g; b = x.b;
 
    bool do_log = true;

    if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;

    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::set( double r, double g, double b)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, r, g, b);
}

OfxStatus rgb_color_param_t::set( OfxTime time, double r, double g, double b)
{
    effect_->node()->block_param_changed_signal( true);
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));

//    bool was_editing_ = true;
//
//    if( !effect_->node()->param_set().editing() && param.can_undo())
//    {
//	was_editing_ = false;
//	effect_->node()->param_set().begin_edit();
//	std::cout << "Command created\n";
//    }

//    if( effect_->node()->overlay()->is_pen_down())
//	effect_->node()->set_dirty( true);

    param.set_value_at_time( Imath::Color4f( r, g, b, 1), time);

//    if( !was_editing_ && !effect_->node()->overlay()->is_pen_down())
//    {
//	effect_->node()->param_set().end_edit( false);
//	std::cout << "Command pushed, !overlay\n";
//    }

    param.update_widgets();

    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::getNumKeys( unsigned int &nKeys) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    nKeys = param.get_num_keys();
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::getKeyTime( int nth, OfxTime& time) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    time = param.get_key_time( nth);
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
    const color_param_t& param( dynamic_cast<const color_param_t&>( effect_->node()->param( getName())));
    param.get_key_index( time, direction, index);

    if( index != -1)
	return kOfxStatOK;
    else
	return kOfxStatFailed;
}

OfxStatus rgb_color_param_t::deleteKey( OfxTime time)
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.delete_key( time);
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::deleteAllKeys()
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.delete_all_keys();
    return kOfxStatOK;
}

} // namespace
} // namespace
