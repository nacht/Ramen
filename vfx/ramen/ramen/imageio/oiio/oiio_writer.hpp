// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_OIIO_WRITER_HPP
#define RAMEN_IMAGEIO_OIIO_WRITER_HPP

#include<ramen/imageio/writer.hpp>

#include<OpenImageIO/imageio.h>
#include<OpenImageIO/typedesc.h>

#include<memory>

namespace ramen
{
namespace imageio
{

class oiio_writer_t : public writer_t
{
public:
    
    oiio_writer_t() : writer_t() {}

protected:

    void do_write( const boost::filesystem::path& p, const OpenImageIO::ImageSpec& spec,
		  const image::const_image_view_t& view);
};

} // namespace
} // namespace

#endif
