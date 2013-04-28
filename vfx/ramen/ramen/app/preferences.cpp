// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/preferences.hpp>

#include<stdlib.h>
#include<stdio.h>

#include<ramen/system/system.hpp>

#include<boost/archive/xml_iarchive.hpp>
#include<boost/archive/xml_oarchive.hpp>

#include<boost/filesystem/fstream.hpp>

#include<ramen/serialization/archive.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{

preferences_impl::preferences_impl()
{
    version_ = 0;
    set_defaults();
    load();
}

void preferences_impl::load()
{
    boost::filesystem::path p = system::system_t::Instance().preferences_path() / "preferences.xml";

    try
    {
	// I think exists shouldn't throw...
	if( !boost::filesystem::exists( p))
	{
	    save();
	    return;
	}
    }
    catch( ...) {}

    boost::filesystem::ifstream ifs( p);

    if( !ifs.is_open())
    {
	save();
	return;
    }

    if( !ifs.good())
    {
	save();
	return;
    }

    boost::archive::xml_iarchive ia( ifs);

    try
    {
	int v;
	ia & boost::serialization::make_nvp( "prefs version", v);

	// default format
	ia & boost::serialization::make_nvp( "default format width", default_format_.width);
	ia & boost::serialization::make_nvp( "default format height", default_format_.height);
	ia & boost::serialization::make_nvp( "default format fps", frame_rate_);

	// memory
	ia & boost::serialization::make_nvp( "max_image_memory", max_image_memory_);
    }
    catch( ...)
    {
	set_defaults();
	save();
    }
}

void preferences_impl::save()
{
    boost::filesystem::path p = system::system_t::Instance().preferences_path() / "preferences.xml";
    boost::filesystem::ofstream ofs( p);

    if( !ofs.is_open())
    {
	application_t::Instance().error( "Couldn't open preferences file for writting. file = " + p.external_file_string());
	return;
    }

    if( !ofs.good())
    {
	application_t::Instance().error( "Couldn't open preferences file for writting. file = " + p.external_file_string());
	return;
    }

    boost::archive::xml_oarchive oa( ofs);

    int v = version_;
    oa & boost::serialization::make_nvp( "prefs_version", v);
    
    // default format
    oa & boost::serialization::make_nvp( "default_format_width", default_format_.width);
    oa & boost::serialization::make_nvp( "default_format_height", default_format_.height);
    oa & boost::serialization::make_nvp( "default_format_fps", frame_rate_);
    
    // memory
    oa & boost::serialization::make_nvp( "max_image_memory", max_image_memory_);
}

void preferences_impl::set_defaults()
{
    max_image_memory_ = 35; // % of ram

    default_format_ = format_t();
    frame_rate_ = 25; // PAL by default.

    // display
    display_gamma_ = 2.2f;

    // Get the display's chromaticities from an environment variable. ( ILM style, taken from CTL)
    // If this fails, assume Rec 709

    static const char chromaticitiesEnv[] = "CTL_DISPLAY_CHROMATICITIES";

    if( const char *chromaticities = getenv( chromaticitiesEnv))
    {
	Imf::Chromaticities tmp;

	int n = sscanf (chromaticities,
			" red %f %f green %f %f blue %f %f white %f %f",
			&tmp.red.x, &tmp.red.y,
			&tmp.green.x, &tmp.green.y,
			&tmp.blue.x, &tmp.blue.y,
			&tmp.white.x, &tmp.white.y);

	if (n == 8)
	    display_chromaticities_ = tmp;
    }

    //
    // Get the display's white luminance from an environment variable.
    // If this fails, assume 120 candelas per square meter.
    // (Screen aim luminance according to SMPTE RP 166.)
    //
    static const char whiteLuminanceEnv[] = "CTL_DISPLAY_WHITE_LUMINANCE";
    display_white_luminance_ = 120.0f;

    if (const char *whiteLuminance = getenv (whiteLuminanceEnv))
	int n = sscanf( whiteLuminance, " %f", &display_white_luminance_);

    static const char surroundLuminanceEnv[] = "CTL_DISPLAY_SURROUND_LUMINANCE";
    display_surround_luminance_ = display_white_luminance_ * 0.1f;

    if (const char *surroundLuminance = getenv (surroundLuminanceEnv))
	int n = sscanf( surroundLuminance, " %f", &display_surround_luminance_);
}

void preferences_impl::set_default_format( int w, int h, int f)
{
    default_format_.width = w;
    default_format_.height = h;
    frame_rate_ = f;
}

} // namespace
