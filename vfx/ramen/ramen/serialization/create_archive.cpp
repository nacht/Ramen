// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/serialization/create_archive.hpp>

#include<boost/archive/polymorphic_xml_iarchive.hpp>
#include<boost/archive/polymorphic_xml_oarchive.hpp>

namespace ramen
{

std::auto_ptr<boost::archive::polymorphic_iarchive> create_iarchive( std::istream& is, unsigned int flags)
{
    boost::archive::polymorphic_iarchive *ia = new boost::archive::polymorphic_xml_iarchive( is, flags);
    return std::auto_ptr<boost::archive::polymorphic_iarchive>( ia);
}

std::auto_ptr<boost::archive::polymorphic_oarchive> create_oarchive( std::ostream& os, unsigned int flags)
{
    boost::archive::polymorphic_oarchive *oa = new boost::archive::polymorphic_xml_oarchive( os, flags);
    return std::auto_ptr<boost::archive::polymorphic_oarchive>( oa);
}

} // namespace
