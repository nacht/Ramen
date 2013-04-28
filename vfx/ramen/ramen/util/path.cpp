// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/util/path.hpp>

#include<sstream>

#include<boost/filesystem/operations.hpp>

#include<QDir>

namespace ramen
{

std::string hash_string( const boost::filesystem::path& p)
{
    std::stringstream s;
    s << p.string();

    try
    {
	// I don't think exists should throw...
	// but it does.
	if( boost::filesystem::exists( p))
	{
	    std::time_t t = boost::filesystem::last_write_time( p);
	    s << t;
	}
    }
    catch( ...) {}

    return s.str();
}

bool is_absolute( const boost::filesystem::path& p) { return p.has_root_path();}
bool is_relative( const boost::filesystem::path& p) { return !is_absolute( p);}

boost::filesystem::path make_absolute_path( const boost::filesystem::path& p, const boost::filesystem::path& from)
{
    QDir dir( QString( from.string().c_str()));
    QString fname( QString( p.string().c_str()));
    QString abs_path( dir.absoluteFilePath( fname));
    return boost::filesystem::path( abs_path.toStdString());
}

boost::filesystem::path make_relative_path( const boost::filesystem::path& p, const boost::filesystem::path& from)
{
    QDir dir( QString( from.string().c_str()));
    QString fname( QString( p.string().c_str()));
    QString rel_path( dir.relativeFilePath( fname));
    return boost::filesystem::path( rel_path.toStdString());
}

} // namespace
