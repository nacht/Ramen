// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EXR_IMATH_COLOR_SERIALIZATION
#define RAMEN_EXR_IMATH_COLOR_SERIALIZATION

#include<OpenEXR/ImathColor.h>

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/level.hpp>

namespace boost
{ 
namespace serialization
{

template<class Archive, class T>
inline void serialize( Archive& ar, Imath::Color3<T>& c, const unsigned int file_version)
{
    ar & boost::serialization::make_nvp( "x", c.x);
    ar & boost::serialization::make_nvp( "y", c.y);
    ar & boost::serialization::make_nvp( "z", c.z);
}

template<class Archive, class T>
inline void serialize( Archive& ar, Imath::Color4<T>& c, const unsigned int file_version)
{
    ar & boost::serialization::make_nvp( "r", c.r);
    ar & boost::serialization::make_nvp( "g", c.g);
    ar & boost::serialization::make_nvp( "b", c.b);
    ar & boost::serialization::make_nvp( "a", c.a);
}

}
}

BOOST_CLASS_IMPLEMENTATION( Imath::Color3c, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::Color3f, boost::serialization::object_serializable);

BOOST_CLASS_IMPLEMENTATION( Imath::Color4c, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::Color4f, boost::serialization::object_serializable);

#endif
