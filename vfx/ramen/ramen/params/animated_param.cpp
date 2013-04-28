// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/animated_param.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/app/document.hpp>
#include<ramen/params/animated_param_command.hpp>

namespace ramen
{

animated_param_t::animated_param_t() : param_t()
{
    step_ = 1.0f;
}

animated_param_t::animated_param_t( const std::string& name) : param_t(name)
{
    step_ = 1.0f;
}

animated_param_t::animated_param_t( const animated_param_t& other) : param_t( other), curves_( other.curves_)
{
    step_ = other.step_;
}

void animated_param_t::copy( const param_t& other, float offset)
{
    param_t::copy( other);
    const animated_param_t *other_ = dynamic_cast<const animated_param_t*>( &other);

    for( int i = 0; i < curves().size(); ++i)
	curves()[i].copy( other_->curves()[i], offset);
}

void animated_param_t::copy( const param_t& other, float offset, float start, float end)
{
    param_t::copy( other);
    const animated_param_t *other_ = dynamic_cast<const animated_param_t*>( &other);

    for( int i = 0; i < curves().size(); ++i)
	curves()[i].copy( other_->curves()[i], offset, start, end);
}

void animated_param_t::add_curve() { curves_.push_back( anim::curve_t());}

void animated_param_t::add_curves( int num)
{
    for( int i = 0; i < num; ++i)
	add_curve();
}

void animated_param_t::set_min( float lo)
{
    adobe::for_each( curves(), boost::bind( &anim::curve_t::set_min, _1, lo));
}

void animated_param_t::set_max( float hi)
{
    adobe::for_each( curves(), boost::bind( &anim::curve_t::set_max, _1, hi));
}

void animated_param_t::set_range( float lo, float hi)
{
    adobe::for_each( curves(), boost::bind( &anim::curve_t::set_range, _1, lo, hi));
}

void animated_param_t::anim_curve_changed( anim::curve_t *c) { do_anim_curve_changed( c);}

float animated_param_t::clamp( float x) const
{
    if( x < get_min()) return get_min();
    if( x > get_max()) return get_max();
    return x;
}

void animated_param_t::do_anim_curve_changed( anim::curve_t *c)
{
    evaluate( document_t::Instance().composition().frame());
    update_widgets();
    notify();
}

std::auto_ptr<undo::command_t> animated_param_t::create_command()
{
    return std::auto_ptr<undo::command_t>( new animated_param_command_t( this));
}

int animated_param_t::get_num_keys() const
{
    if( curves().size() == 1)
	return curves()[0].size();

    // this is not very efficient.
    Loki::AssocVector<float, adobe::empty_t> key_times;
    fill_key_times_assoc_vector( key_times);
    return key_times.size();
}

float animated_param_t::get_key_time( int nth) const
{
    if( curves().size() == 1)
	return curves()[0][nth].time();

    Loki::AssocVector<float, adobe::empty_t> key_times;
    fill_key_times_assoc_vector( key_times);
    return ( key_times.begin())[nth].first;
}

void animated_param_t::get_key_index( float time, int direction, int& index) const
{
    Loki::AssocVector<float, adobe::empty_t> key_times;
    fill_key_times_assoc_vector( key_times);

    index = -1;

    if( key_times.empty())
	return;

    if( direction < 0) // search before time
    {
        Loki::AssocVector<float, adobe::empty_t>::const_iterator it( key_times.lower_bound( time));

	if( it != key_times.begin())
	    index = it - key_times.begin() - 1;
    }
    else
    {
	if( direction > 0) // search after time
	{
            Loki::AssocVector<float, adobe::empty_t>::const_iterator it( key_times.lower_bound( time));

            if( it != key_times.end())
		index = it - key_times.begin() - 1;
	}
        else // search around time
	{
            // TODO: binary search would be more efficient for this,
            // but for now, this is easier to debug

            const float time_tol = 0.5f;
            int ix = 0;

            for( Loki::AssocVector<float, adobe::empty_t>::const_iterator it( key_times.begin()); it != key_times.end(); ++it, ++ix)
            {
                if( Imath::abs( it->first - time) < time_tol)
                {
                    index = ix;
                    return;
                }
            }
	}
    }
}

void animated_param_t::delete_key( float time)
{
    BOOST_FOREACH( anim::curve_t& c, curves()) { c.erase( time);}
}

void animated_param_t::delete_all_keys()
{
    BOOST_FOREACH( anim::curve_t& c, curves()) { c.clear();}
}

void animated_param_t::fill_key_times_assoc_vector( Loki::AssocVector<float, adobe::empty_t>& v) const
{
    BOOST_FOREACH( const anim::curve_t& c, curves())
    {
	BOOST_FOREACH( const anim::keyframe_t& k, c.keys())
	{
	    v.insert( std::make_pair( k.time(), adobe::empty_t()));
	}
    }
}

void animated_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
    ar & boost::serialization::make_nvp( "curves", curves_);
    ar & boost::serialization::make_nvp( "step", step_);
}

void animated_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
    ar & boost::serialization::make_nvp( "curves", curves_);
    ar & boost::serialization::make_nvp( "step", step_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::animated_param_t)
