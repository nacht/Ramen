// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/multichannel_reader.hpp>

namespace ramen
{
namespace imageio
{

multichannel_reader_t::multichannel_reader_t( const boost::filesystem::path& p) : reader_t( p)
{
    has_extra_channels_ = false;
}

void multichannel_reader_t::add_channel_name( const std::string& c) { channels_.push_back( c);}

void multichannel_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    multichannel_reader_t::read_image( view, crop, subsample, boost::make_tuple( 0, 1, 2, 3));
}

} // namespace
} // namespace
