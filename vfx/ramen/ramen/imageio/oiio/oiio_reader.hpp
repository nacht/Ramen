// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_OIIO_READER_HPP
#define	RAMEN_IMAGEIO_OIIO_READER_HPP

#include<ramen/imageio/reader.hpp>

namespace ramen
{
namespace imageio
{

class oiio_reader_t : public reader_t
{
public:

    oiio_reader_t( const boost::filesystem::path& p, float default_gamma = 1.0f);

private:

    virtual void do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const;

    float gamma_;
};

} // namespace
} // namespace

#endif	/* _oiio_READER_HPP */
