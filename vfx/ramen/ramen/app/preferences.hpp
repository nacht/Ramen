// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PREFERENCES_HPP
#define RAMEN_PREFERENCES_HPP

#include<vector>

#include<boost/noncopyable.hpp>
#include<boost/filesystem/path.hpp>

#include<loki/Singleton.h>

#include<OpenEXR/ImfChromaticities.h>

#include<ramen/util/signal.hpp>
#include<ramen/util/format.hpp>

namespace ramen
{

class preferences_impl : boost::noncopyable
{
public:

    boost::signal<void ()> changed;
    void notify() { changed();}

    // memory
    int max_image_memory() const        { return max_image_memory_;}
    void set_max_image_memory( int m)   { max_image_memory_ = m;}

    // display
    float display_gamma() const         { return display_gamma_;}
    void set_display_gamma( float g)    { display_gamma_ = g;}

    const Imf::Chromaticities& display_chromaticities() const       { return display_chromaticities_;}
    void set_display_chromaticities( const Imf::Chromaticities& c)  { display_chromaticities_ = c;}

    float display_white_luminance() const       { return display_white_luminance_;}
    void set_display_white_luminance( float l)  { display_white_luminance_ = l;}
    
    float display_surround_luminance() const        { return display_surround_luminance_;}
    void set_display_surround_luminance( float l)   { display_surround_luminance_ = l;}

    // format
    void set_default_format( int w, int h, int f);
    format_t default_format() const { return default_format_;}

    int frame_rate() const      { return frame_rate_;}
    void set_frame_rate( int f) { frame_rate_ = f;}

    void set_defaults();

    void save();

private:

    friend struct Loki::CreateUsingNew<preferences_impl>;

    preferences_impl();

    void load();

    int version_;

    format_t default_format_;
    int frame_rate_;

    int max_image_memory_;

    // display
    float display_gamma_;
    Imf::Chromaticities display_chromaticities_;
    float display_white_luminance_;
    float display_surround_luminance_;
};

typedef Loki::SingletonHolder<preferences_impl> preferences_t;

} // namespace

#endif
