// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EXR_IMATH_BOX_SERIALIZATION
#define RAMEN_EXR_IMATH_BOX_SERIALIZATION

#include<OpenEXR/ImathBox.h>

#include<ramen/ImathExt/serialization/ImathVec.hpp>

namespace boost
{ 
namespace serialization
{

template<class Archive, class T>
inline void serialize( Archive& ar, Imath::Box<T>& b, const unsigned int file_version)
{
    ar & boost::serialization::make_nvp( "min", b.min);
    ar & boost::serialization::make_nvp( "max", b.max);
}

}
}

BOOST_CLASS_IMPLEMENTATION( Imath::Box2i, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::Box2f, boost::serialization::object_serializable);

BOOST_CLASS_IMPLEMENTATION( Imath::Box3i, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION( Imath::Box3f, boost::serialization::object_serializable);

#endif
