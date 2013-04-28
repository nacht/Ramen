// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CREATE_ARCHIVE_HPP
#define	RAMEN_CREATE_ARCHIVE_HPP

#include<memory>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

namespace ramen
{

std::auto_ptr<boost::archive::polymorphic_iarchive> create_iarchive( std::istream& is, unsigned int flags = 0);
std::auto_ptr<boost::archive::polymorphic_oarchive> create_oarchive( std::ostream& os, unsigned int flags = 0);

} // namespace

#endif	/* _ARCHIVE_HPP */
