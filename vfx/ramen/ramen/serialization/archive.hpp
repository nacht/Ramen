// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ARCHIVE_HPP
#define	RAMEN_ARCHIVE_HPP

#include<boost/archive/polymorphic_xml_iarchive.hpp>
#include<boost/archive/polymorphic_xml_oarchive.hpp>

#include<boost/filesystem/fstream.hpp>

namespace ramen
{

typedef boost::archive::polymorphic_xml_iarchive input_archive_t;
typedef boost::archive::polymorphic_xml_oarchive output_archive_t;

typedef boost::filesystem::ifstream ifstream_t;
typedef boost::filesystem::ofstream ofstream_t;

} // namespace

#endif	/* _ARCHIVE_HPP */
