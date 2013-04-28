// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/static_param.hpp>

#include<ramen/params/static_param_command.hpp>

namespace ramen
{

static_param_t::static_param_t() : param_t() { set_static( true);}
static_param_t::static_param_t( const std::string& name) : param_t(name) { set_static( true);}

static_param_t::static_param_t( const static_param_t& other) : param_t( other) {}

std::auto_ptr<undo::command_t> static_param_t::create_command()
{
    return std::auto_ptr<undo::command_t>( new static_param_command_t( this));
}

std::auto_ptr<undo::command_t> static_param_t::create_reset_command()
{
    return std::auto_ptr<undo::command_t>( new static_param_reset_command_t( this));
}

void static_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

void static_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::static_param_t)
