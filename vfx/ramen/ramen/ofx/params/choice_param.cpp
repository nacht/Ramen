// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/choice_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

choice_param_t::choice_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::ChoiceInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus choice_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    const choice_param_t *other = dynamic_cast<const choice_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus choice_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    const choice_param_t *other = dynamic_cast<const choice_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus choice_param_t::get( int& v) { return get( 1.0, v);}

OfxStatus choice_param_t::get( OfxTime time, int& v)
{
    v = get_value<int>( effect_->node()->param( getName()));

    bool do_log = true;

    if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;
    
    return kOfxStatOK;
}

OfxStatus choice_param_t::set(int v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus choice_param_t::set(OfxTime time, int v)
{
    effect_->node()->block_param_changed_signal( true);
    popup_param_t& param( dynamic_cast<popup_param_t&>( effect_->node()->param( getName())));
    param.set_value( v);
    param.update_widgets();
    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

} // namespace
} // namespace
