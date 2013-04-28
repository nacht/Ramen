// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/param.hpp>

#include<stdexcept>

#include<boost/serialization/string.hpp>

#include<ramen/params/parameterised.hpp>

namespace ramen
{

param_t::param_t() : QObject(), param_set_(0)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit;
}

param_t::param_t( const std::string& name) : QObject(), param_set_(0), name_(name)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit;
}

param_t::param_t( const param_t& other) : QObject(), param_set_(0), id_( other.id_), name_( other.name_),
					    value_( other.value_), default_( other.default_), flags_( other.flags_)
{
}

void param_t::init() { do_init();}

void param_t::copy( const param_t& other, float offset)
{
    assert( typeid( this) == typeid( &other) && "Copy called for different typed params");
    value_ = other.value_;
    default_ = other.default_;
}

void param_t::copy( const param_t& other, float offset, float start, float end) { copy( other);}

void param_t::set_param_set( param_set_t *parent) { param_set_ = parent;}

const parameterised_t *param_t::parameterised() const
{
    assert( param_set());
    return param_set()->parent();
}

parameterised_t *param_t::parameterised()
{
    assert( param_set());
    return param_set()->parent();
}

const composition_t *param_t::composition() const
{
    assert( param_set());
    const parameterised_t *p = parameterised();

    if( p)
	return p->composition();
    else
	return 0;
}

composition_t *param_t::composition()
{
    assert( param_set());
    parameterised_t *p = parameterised();

    if( p)
	return p->composition();
    else
	return 0;
}

void param_t::set_enabled( bool e)
{
    if( e)
	flags_ |= enabled_bit;
    else
	flags_ &= ~enabled_bit;

    enable_widgets( enabled());
}

void param_t::set_static( bool b)
{
    if( b)
	flags_ |= static_bit;
    else
	flags_ &= ~static_bit;
}

void param_t::set_secret( bool b)
{
    if( b)
	flags_ |= secret_bit;
    else
	flags_ &= ~secret_bit;
}

void param_t::set_persist( bool b)
{
    if( b)
	flags_ |= persist_bit;
    else
	flags_ &= ~persist_bit;

}

void param_t::set_can_undo( bool b)
{
    if( b)
	flags_ |= can_undo_bit;
    else
	flags_ &= ~can_undo_bit;
}

void param_t::set_track_mouse( bool b)
{
    if( b)
	flags_ &= ~dont_track_mouse_bit;
    else
	flags_ |= dont_track_mouse_bit;
}

bool param_t::has_value() const { return !adobe::empty( value());}

adobe::any_regular_t param_t::do_get_value_at_time( float time) const { return value();}

void param_t::notify()
{
    assert( param_set_);
    param_set()->param_changed( this);
}

QWidget *param_t::create_widgets()
{
    if( !secret())
        return do_create_widgets();

    return 0;
}
    
std::string param_t::str() const { return do_str();}

std::string param_t::do_str() const { return std::string();}

void param_t::evaluate( float time) { do_evaluate( time);}

void param_t::do_evaluate( float time) {}

void param_t::create_tracks( anim::track_t *parent)
{
    if( !is_static())
	do_create_tracks( parent);
}

void param_t::serialize_values( boost::archive::polymorphic_iarchive& ar)
{
    ar & boost::serialization::make_nvp( "value", value_);
    ar & boost::serialization::make_nvp( "default", default_);
}

void param_t::serialize_values( boost::archive::polymorphic_oarchive& ar) const
{
    // if not persistent, then save the default value as the current one
    if( persist())
	ar & boost::serialization::make_nvp( "value", value_);
    else
	ar & boost::serialization::make_nvp( "value", default_);

    ar & boost::serialization::make_nvp( "default", default_);
}

void param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_NVP( name_);
    ar & BOOST_SERIALIZATION_NVP( id_);
    ar & BOOST_SERIALIZATION_NVP( flags_);
    serialize_values( ar);
}

void param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_NVP( name_);
    ar & BOOST_SERIALIZATION_NVP( id_);
    ar & BOOST_SERIALIZATION_NVP( flags_);
    serialize_values( ar);
}

param_t* new_clone( const param_t& other) { return other.clone();}

} // namespace

BOOST_CLASS_EXPORT( ramen::param_t)
