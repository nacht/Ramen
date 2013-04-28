// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_CINEON_WRITER_HPP
#define RAMEN_IMAGEIO_CINEON_WRITER_HPP

#include<ramen/imageio/writer.hpp>

#include<fstream>

namespace ramen
{
namespace imageio
{

class cineon_writer_t : public writer_t
{
public:
    
    cineon_writer_t() : writer_t() {}

private:

    virtual void do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const;

    void write_pixels( std::ofstream& out, const image::const_image_view_t& view) const;
};

} // namespace
} // namespace

#endif
