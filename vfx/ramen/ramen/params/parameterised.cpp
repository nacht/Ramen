// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/parameterised.hpp>

#include<boost/ptr_container/serialize_ptr_vector.hpp>
#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

namespace ramen
{

parameterised_t::parameterised_t() : dont_persist_params_( false), autokey_( true)
{
    set_composition( 0);
    params_.set_parent( this);
}

parameterised_t::parameterised_t( const parameterised_t& other) : params_( other.params_)
{
    composition_ = other.composition_;
    dont_persist_params_ = other.dont_persist_params_;
    autokey_ = other.autokey_;
    params_.set_parent( this);
}

parameterised_t::~parameterised_t() { released_( this);}

void parameterised_t::create_params()
{
    do_create_params();
    adobe::for_each( param_set(), boost::bind( &param_t::init, _1));
}

void parameterised_t::do_create_params() {}

const param_t& parameterised_t::param( const std::string& identifier) const
{
    return param_set().find( identifier);
}

param_t& parameterised_t::param( const std::string& identifier)
{
    return param_set().find( identifier);
}

void parameterised_t::evaluate( float time)
{
    adobe::for_each( param_set(), boost::bind( &param_t::evaluate, _1, time));
}

void parameterised_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "dont_persist_params", dont_persist_params_);
    ar & boost::serialization::make_nvp( "autokey", autokey_);

    if( !dont_persist_params())
	ar & boost::serialization::make_nvp( "params", params_);
    else
	create_params();

    param_set().set_parent( this);
}

void parameterised_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & boost::serialization::make_nvp( "dont_persist_params", dont_persist_params_);
    ar & boost::serialization::make_nvp( "autokey", autokey_);

    if( !dont_persist_params())
	ar & boost::serialization::make_nvp( "params", params_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::parameterised_t)
