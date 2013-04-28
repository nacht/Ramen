// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/double_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

double_param_t::double_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::DoubleInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus double_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset)
{
    const double_param_t *other = dynamic_cast<const double_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus double_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset, OfxRangeD range)
{
    const double_param_t *other = dynamic_cast<const double_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus double_param_t::get( double& v)
{
    v = get_value<float>( effect_->node()->param( getName()));
    return kOfxStatOK;
}

OfxStatus double_param_t::get( OfxTime time, double& v)
{
    v = get_value_at_time<float>( effect_->node()->param( getName()), time);

    bool do_log = true;

    if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;
    
    return kOfxStatOK;
}

OfxStatus double_param_t::set( double v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus double_param_t::set( OfxTime time, double v)
{
    effect_->node()->block_param_changed_signal( true);
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_time( v, time);
    param.update_widgets();
    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

OfxStatus double_param_t::derive( OfxTime time, double& v)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    v = param.derive( time);
    return kOfxStatOK;
}

OfxStatus double_param_t::integrate( OfxTime time1, OfxTime time2, double& v)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    v = param.integrate( time1, time2);
    return kOfxStatOK;
}

OfxStatus double_param_t::getNumKeys( unsigned int &nKeys) const
{
    const float_param_t& param( dynamic_cast<const float_param_t&>( effect_->node()->param( getName())));
    nKeys = param.get_num_keys();
    return kOfxStatOK;
}

OfxStatus double_param_t::getKeyTime( int nth, OfxTime& time) const
{
    const float_param_t& param( dynamic_cast<const float_param_t&>( effect_->node()->param( getName())));
    time = param.get_key_time( nth);
    return kOfxStatOK;
}

OfxStatus double_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
    const float_param_t& param( dynamic_cast<const float_param_t&>( effect_->node()->param( getName())));
    param.get_key_index( time, direction, index);

    if( index != -1)
	return kOfxStatOK;
    else
	return kOfxStatFailed;
}

OfxStatus double_param_t::deleteKey( OfxTime time)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    param.delete_key( time);
    return kOfxStatOK;
}

OfxStatus double_param_t::deleteAllKeys()
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    param.delete_all_keys();
    return kOfxStatOK;
}

} // namespace
} // namespace
