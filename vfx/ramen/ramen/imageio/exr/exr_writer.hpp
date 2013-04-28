// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_EXR_WRITER_HPP
#define RAMEN_IMAGEIO_EXR_WRITER_HPP

#include<ramen/imageio/writer.hpp>

#include<OpenEXR/ImfHeader.h>

namespace ramen
{
namespace imageio
{

class exr_writer_t : public writer_t
{
public:
    
    exr_writer_t() : writer_t() {}

private:

    virtual void do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const;

    void write_half( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha = true) const;
    void write_half_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const;

    void write_float( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha = true) const;
    void write_float_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const;

    void write_multilevel_half( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha = true) const;
    void write_multilevel_half_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const;

    void write_multilevel_float( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view, bool save_alpha = true) const;
    void write_multilevel_float_alpha( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view) const;
};

} // namespace
} // namespace

#endif
