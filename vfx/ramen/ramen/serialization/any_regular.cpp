// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/serialization/any_regular.hpp>

#include<stdlib.h>

#include<iostream>

namespace ramen
{

any_regular_serialization_registry_impl::any_regular_serialization_registry_impl()
{
    register_load_function( adobe::type_info<adobe::empty_t>().name(), &any_regular_serialization_registry_impl::load_empty);
    register_save_function( adobe::type_info<adobe::empty_t>().name(), &any_regular_serialization_registry_impl::save_empty);

    register_type<double>();
    register_type<bool>();
}

bool any_regular_serialization_registry_impl::register_load_function( const std::string& type, const ramen::any_regular_load_function_t& fun)
{
    load_functions_[type] = fun;
    return true;
}

bool any_regular_serialization_registry_impl::register_save_function( const std::string& type, const ramen::any_regular_save_function_t& fun)
{
    save_functions_[type] = fun;
    return true;
}

void any_regular_serialization_registry_impl::load_empty( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& v,
							 const unsigned int version)
{
    v.assign( adobe::empty_t());
}

void any_regular_serialization_registry_impl::save_empty( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& v,
							 const unsigned int version) {}

ramen::any_regular_load_function_t& any_regular_serialization_registry_impl::load_function( const std::string& type)
{
    std::map<std::string, ramen::any_regular_load_function_t>::iterator it( load_functions_.find( type));
    
    if( it != load_functions_.end())
	return it->second;

    std::cout << "Any regular serialization load: unregistered type " << type << "\n";
    abort();
}

ramen::any_regular_save_function_t& any_regular_serialization_registry_impl::save_function( const std::string& type)
{
    std::map<std::string, ramen::any_regular_save_function_t>::iterator it( save_functions_.find( type));

    if( it != save_functions_.end())
	return it->second;

    std::cout << "Any regular serialization save: unregistered type " << type << "\n";
    abort();
}

} // namespace

namespace boost
{
namespace serialization
{

void load( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& any, const unsigned int version)
{
    std::string type;
    ar & BOOST_SERIALIZATION_NVP( type);
    ramen::any_regular_serialization_registry_t::Instance().load_function( type)( ar, any, version);
}

void save( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& any, const unsigned int version)
{
    std::string type = any.type_info().name();
    ar & BOOST_SERIALIZATION_NVP( type);
    ramen::any_regular_serialization_registry_t::Instance().save_function( type)( ar, any, version);
}

} // namespace
} // namespace
