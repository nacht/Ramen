// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_EXR_READER_HPP
#define	RAMEN_IMAGEIO_EXR_READER_HPP

#include<ramen/imageio/multichannel_reader.hpp>

#include<OpenEXR/ImfHeader.h>

namespace ramen
{
namespace imageio
{

class exr_reader_t : public multichannel_reader_t
{
public:

    exr_reader_t( const boost::filesystem::path& p);

private:

    virtual void do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample,
                        const boost::tuple<int,int,int,int>& channels) const;

private:

    bool is_rgb() const;
    bool is_luma_chroma() const;

    Imf::Header header_;
    bool is_rgb_image_;
    bool is_luma_chroma_image_;
    bool multilevel_;
};
    
} // namespace    
} // namespace

#endif	/* _EXR_READER_HPP */
