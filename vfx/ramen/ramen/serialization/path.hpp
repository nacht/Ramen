// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef BOOST_EXT_PATH_SERIALIZATION_HPP
#define BOOST_EXT_PATH_SERIALIZATION_HPP

#include<boost/filesystem/path.hpp>
#include<boost/serialization/string.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/split_free.hpp>

namespace boost
{
namespace serialization
{

template<class Archive>
void save( Archive& ar, const boost::filesystem::path& p, const unsigned int version)
{
    std::string x( p.string());
    ar & boost::serialization::make_nvp( "path", x);
}

template<class Archive>
void load( Archive& ar, boost::filesystem::path& p, const unsigned int version)
{
    std::string x;
    ar & boost::serialization::make_nvp( "path", x);
    p = boost::filesystem::path( x);
}

template<class Archive>
void save( Archive& ar, const boost::filesystem::wpath& p, const unsigned int version)
{
    std::wstring x( p.string());
    ar & boost::serialization::make_nvp( "path", x);
}

template<class Archive>
void load( Archive& ar, boost::filesystem::wpath& p, const unsigned int version)
{
    std::wstring x;
    ar & boost::serialization::make_nvp( "path", x);
    p = boost::filesystem::wpath( x);
}

}
}

BOOST_SERIALIZATION_SPLIT_FREE( boost::filesystem::path)
BOOST_SERIALIZATION_SPLIT_FREE( boost::filesystem::wpath)

#endif

