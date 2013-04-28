// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SERIALIZATION_ANY_REGULAR_HPP
#define	RAMEN_SERIALIZATION_ANY_REGULAR_HPP

#include<map>

#include<boost/noncopyable.hpp>
#include<boost/serialization/string.hpp>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

#include<boost/serialization/split_free.hpp>

#include<boost/function.hpp>

#include<loki/Singleton.h>

#include<adobe/any_regular.hpp>

namespace boost
{
namespace serialization
{

void load( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& any, const unsigned int version);
void save( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& any, const unsigned int version);

} // namespace
} // namespace

namespace ramen
{

typedef boost::function<void ( boost::archive::polymorphic_iarchive&,
				adobe::any_regular_t&, const unsigned int)> any_regular_load_function_t;

typedef boost::function<void ( boost::archive::polymorphic_oarchive&,
				const adobe::any_regular_t&, const unsigned int)> any_regular_save_function_t;

class any_regular_serialization_registry_impl : boost::noncopyable
{
public:

    bool register_load_function( const std::string& type, const ramen::any_regular_load_function_t& fun);
    bool register_save_function( const std::string& type, const ramen::any_regular_save_function_t& fun);

    template<class T>
    bool register_type()
    {
        register_load_function( adobe::type_info<T>().name(), load_value<T>);
        register_save_function( adobe::type_info<T>().name(), save_value<T>);
        return true;
    }

private:

    any_regular_serialization_registry_impl();

    ramen::any_regular_load_function_t& load_function( const std::string& type);
    ramen::any_regular_save_function_t& save_function( const std::string& type);

    static void load_empty( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& v, const unsigned int version);
    static void save_empty( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& v, const unsigned int version);

    template<class T>
    static void load_value( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& v, const unsigned int version)
    {
        T value;
        ar & BOOST_SERIALIZATION_NVP( value);
        v.assign( value);
    }

    template<class T>
    static void save_value( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& v, const unsigned int version)
    {
        T value = v.cast<T>();
        ar & BOOST_SERIALIZATION_NVP( value);
    }

    friend struct Loki::CreateUsingNew<any_regular_serialization_registry_impl>;

    friend void boost::serialization::load( boost::archive::polymorphic_iarchive& ar, adobe::any_regular_t& any, const unsigned int version);
    friend void boost::serialization::save( boost::archive::polymorphic_oarchive& ar, const adobe::any_regular_t& any, const unsigned int version);

    std::map<std::string, ramen::any_regular_load_function_t> load_functions_;
    std::map<std::string, ramen::any_regular_save_function_t> save_functions_;
};

typedef Loki::SingletonHolder<any_regular_serialization_registry_impl> any_regular_serialization_registry_t;


} // namespace

BOOST_SERIALIZATION_SPLIT_FREE( adobe::any_regular_t)

#endif	/* _ANY_REGULAR_HPP */
