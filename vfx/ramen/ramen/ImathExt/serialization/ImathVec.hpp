// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EXR_IMATH_VEC_SERIALIZATION
#define RAMEN_EXR_IMATH_VEC_SERIALIZATION

#include<OpenEXR/ImathVec.h>

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/level.hpp>

namespace boost
{ 
namespace serialization
{

template<class Archive, class T>
inline void serialize( Archive& ar, Imath::Vec2<T>& v, const unsigned int file_version)
{
    ar & boost::serialization::make_nvp( "x", v.x);
    ar & boost::serialization::make_nvp( "y", v.y);
}

template<class Archive, class T>
inline void serialize( Archive& ar, Imath::Vec3<T>& v, const unsigned int file_version)
{
    ar & boost::serialization::make_nvp( "x", v.x);
    ar & boost::serialization::make_nvp( "y", v.y);
    ar & boost::serialization::make_nvp( "z", v.z);
}

}
}

BOOST_CLASS_IMPLEMENTATION( Imath::V2i, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::V2f, boost::serialization::object_serializable);

BOOST_CLASS_IMPLEMENTATION( Imath::V3i, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::V3f, boost::serialization::object_serializable);

#endif
