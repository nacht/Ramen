// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_READER_HPP
#define	RAMEN_IMAGEIO_READER_HPP

#include<vector>
#include<string>

#include<boost/filesystem/path.hpp>
#include<boost/cstdint.hpp>

#include<OpenEXR/ImathBox.h>

#include<adobe/dictionary.hpp>

#include<ramen/imageio/exceptions.hpp>
#include<ramen/imageio/enums.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace imageio
{

class reader_t
{
public:

    reader_t( const boost::filesystem::path& p);
    virtual ~reader_t() {};

    const adobe::dictionary_t& image_info() const;

    Imath::Box2i domain() const;
    Imath::Box2i defined() const;

    void read_image( const image::image_view_t& view) const;

    void read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const;

private:

    virtual void do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const = 0;

protected:

    // some utility functions
    void repeat_scanline_until_end( const image::image_view_t& view, int y) const;

    void apply_gamma( const image::const_image_view_t& src, const image::image_view_t& dst, float gamma) const;

    boost::filesystem::path path_;
    adobe::dictionary_t info_;
};

} // namespace
} // namespace

#endif
