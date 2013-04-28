// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/params/double2d_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/float2_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

double2d_param_t::double2d_param_t( const std::string& name,
				   OFX::Host::Param::Descriptor& descriptor,
				   OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::Double2DInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus double2d_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    const double2d_param_t *other = dynamic_cast<const double2d_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset);
    return kOfxStatOK;
}

OfxStatus double2d_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    const double2d_param_t *other = dynamic_cast<const double2d_param_t*>( &instance);
    effect_->node()->param( getName()).copy( other->effect_->node()->param( other->getName()), offset, range.min, range.max);
    return kOfxStatOK;
}

OfxStatus double2d_param_t::get( double& x, double& y)
{
    Imath::V2f v( get_value<Imath::V2f>( effect_->node()->param( getName())));
    x = v.x; y = v.y;
    return kOfxStatOK;
}

OfxStatus double2d_param_t::get( OfxTime time, double& x, double& y)
{
    Imath::V2f v( get_value_at_time<Imath::V2f>( effect_->node()->param( getName()), time));
    x = v.x; y = v.y;

    bool do_log = true;

    if( effect_->node()->overlay() && effect_->node()->overlay()->is_pen_moving())
	do_log = false;

    return kOfxStatOK;
}

OfxStatus double2d_param_t::set( double x, double y)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
	t = effect_->node()->composition()->frame();

    return set( t, x, y);
}

OfxStatus double2d_param_t::set( OfxTime time, double x, double y)
{
    effect_->node()->block_param_changed_signal( true);
    float2_param_t& param( dynamic_cast<float2_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_time( Imath::V2f( x, y), time);
    param.update_widgets();
    effect_->node()->block_param_changed_signal( false);
    return kOfxStatOK;
}

OfxStatus double2d_param_t::getNumKeys(unsigned int &nKeys) const
{
    const float2_param_t& param( dynamic_cast<const float2_param_t&>( effect_->node()->param( getName())));
    nKeys = param.get_num_keys();
    return kOfxStatOK;
}

OfxStatus double2d_param_t::getKeyTime(int nth, OfxTime& time) const
{
    const float2_param_t& param( dynamic_cast<const float2_param_t&>( effect_->node()->param( getName())));
    time = param.get_key_time( nth);
    return kOfxStatOK;
}

OfxStatus double2d_param_t::getKeyIndex(OfxTime time, int direction, int & index) const
{
    const float2_param_t& param( dynamic_cast<const float2_param_t&>( effect_->node()->param( getName())));
    param.get_key_index( time, direction, index);

    if( index != -1)
	return kOfxStatOK;
    else
	return kOfxStatFailed;
}

OfxStatus double2d_param_t::deleteKey(OfxTime time)
{
    float2_param_t& param( dynamic_cast<float2_param_t&>( effect_->node()->param( getName())));
    param.delete_key( time);
    return kOfxStatOK;
}

OfxStatus double2d_param_t::deleteAllKeys()
{
    float2_param_t& param( dynamic_cast<float2_param_t&>( effect_->node()->param( getName())));
    param.delete_all_keys();
    return kOfxStatOK;
}

} // namespace
} // namespace
