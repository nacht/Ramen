// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef BOOST_EXT_TUPLE_SERIALIZATION_HPP
#define	BOOST_EXT_TUPLE_SERIALIZATION_HPP

#include<boost/tuple/tuple.hpp>
#include<boost/serialization/nvp.hpp>

namespace boost
{
namespace serialization
{

template<class Archive, class T0, class T1>
void serialize( Archive& ar, boost::tuple<T0, T1>& t, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "first"    , t.get<0>());
    ar & boost::serialization::make_nvp( "second"   , t.get<1>());
}

template<class Archive, class T0, class T1, class T2>
void serialize( Archive& ar, boost::tuple<T0, T1, T2>& t, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "first"    , t.get<0>());
    ar & boost::serialization::make_nvp( "second"   , t.get<1>());
    ar & boost::serialization::make_nvp( "third"    , t.get<2>());
}

template<class Archive, class T0, class T1, class T2, class T3>
void serialize( Archive& ar, boost::tuple<T0, T1, T2, T3>& t, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "first"    , t.get<0>());
    ar & boost::serialization::make_nvp( "second"   , t.get<1>());
    ar & boost::serialization::make_nvp( "third"    , t.get<2>());
    ar & boost::serialization::make_nvp( "fourth"   , t.get<3>());
}

} // namespace
} // namespace

#endif	/* _TUPLE_HPP */

