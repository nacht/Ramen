// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

/*
    serialization for adobe copy_on_write
    assumes copy_on_write object being serialized
    is unique.
 */

#ifndef RAMEN_COPY_ON_WRITE_SERIALIZATION_HPP
#define	RAMEN_COPY_ON_WRITE_SERIALIZATION_HPP

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/split_free.hpp>

#include<adobe/copy_on_write.hpp>

namespace boost
{
namespace serialization
{

template<class Archive, class T, class A>
void load( Archive& ar, adobe::copy_on_write<T,A>& cow, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "value", cow.write());
}

template<class Archive, class T, class A>
void save( Archive& ar, const adobe::copy_on_write<T,A>& cow, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "value", cow.read());
}

template<class Archive, class T, class A>
inline void serialize( Archive& ar, adobe::copy_on_write<T,A>& cow, const unsigned int version)
{
    split_free( ar, cow, version);
}

} // namespace
} // namespace

#endif	/* _COPY_ON_WRITE_HPP */
