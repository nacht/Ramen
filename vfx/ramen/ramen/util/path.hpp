// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BOOST_PATH_HPP
#define RAMEN_BOOST_PATH_HPP

#include<boost/filesystem/path.hpp>

namespace ramen
{

std::string hash_string( const boost::filesystem::path& p);

bool is_absolute( const boost::filesystem::path& p);
bool is_relative( const boost::filesystem::path& p);

boost::filesystem::path make_absolute_path( const boost::filesystem::path& p, const boost::filesystem::path& from);
boost::filesystem::path make_relative_path( const boost::filesystem::path& p, const boost::filesystem::path& from);

} // namespace

#endif
