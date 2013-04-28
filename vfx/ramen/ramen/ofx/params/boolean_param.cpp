// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/boolean_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

boolean_param_t::boolean_param_t( const std::string& name,
			   OFX::Host::Param::Descriptor& descriptor,
			   OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::BooleanInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus boolean_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    const boolean_param_t *other = dynamic_cast<const boolean_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus boolean_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    const boolean_param_t *other = dynamic_cast<const boolean_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus boolean_param_t::get( bool& v) { return get( 1.0, v);}

OfxStatus boolean_param_t::get( OfxTime time, bool& v)
{
    v = get_value<bool>( effect_->node()->param( getName()));

    bool do_log = true;

    if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;
    
    return kOfxStatOK;
}

OfxStatus boolean_param_t::set( bool v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus boolean_param_t::set (OfxTime time, bool v)
{
    effect_->node()->block_param_changed_signal( true);

    bool_param_t& param( dynamic_cast<bool_param_t&>( effect_->node()->param( getName())));
//    bool was_editing_ = true;
//
//    if( !effect_->node()->param_set().editing() && param.can_undo())
//    {
//	was_editing_ = false;
//	effect_->node()->param_set().begin_edit();
//    }
//
//    if( effect_->node()->overlay()->is_pen_down())
//	effect_->node()->set_dirty( true);

    param.set_value( v);

//    if( !was_editing_ && !effect_->node()->overlay()->is_pen_down())
//	effect_->node()->param_set().end_edit( false);

    param.update_widgets();

    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

} // namespace
} // namespace
