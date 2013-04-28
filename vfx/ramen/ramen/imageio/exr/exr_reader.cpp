// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exr/exr_reader.hpp>

#include<OpenEXR/ImfTestFile.h>
#include<OpenEXR/ImfInputFile.h>
#include<OpenEXR/ImfTiledInputFile.h>
#include<OpenEXR/ImfChannelList.h>

#include<ramen/imageio/read_exr.hpp>

namespace ramen
{
namespace imageio
{

exr_reader_t::exr_reader_t( const boost::filesystem::path& p) : multichannel_reader_t( p), multilevel_( false)
{
    bool tiled;

    if(!Imf::isOpenExrFile( p.external_file_string().c_str(), tiled))
	throw std::runtime_error( "Can't open file");

    if( tiled)
    {
	Imf::TiledInputFile ifile( p.external_file_string().c_str());
	header_ = ifile.header();

	if( ifile.levelMode() != Imf::ONE_LEVEL)
	    multilevel_ = true;
    }
    else
    {
	Imf::InputFile ifile( p.external_file_string().c_str());
	header_ = ifile.header();
    }

    Imath::Box2i display_window( header_.displayWindow());
    info_[adobe::name_t( "domain")] = adobe::any_regular_t( display_window);

    Imath::Box2i data_window( header_.dataWindow());
    info_[adobe::name_t( "defined")] = adobe::any_regular_t( data_window);

    is_rgb_image_ = is_rgb();

    if( is_rgb_image_)
    {
	add_channel_name( "R");
	add_channel_name( "G");
	add_channel_name( "B");
	add_channel_name( "A");
	is_luma_chroma_image_ = false;
    }
    else
    {
	is_luma_chroma_image_ = is_luma_chroma();

	if( is_luma_chroma_image_)
	{
	    add_channel_name( "R");
	    add_channel_name( "G");
	    add_channel_name( "B");
	    add_channel_name( "A");
	}
    }

    // add channel names
    const Imf::ChannelList& ch_list( header_.channels());

    for( Imf::ChannelList::ConstIterator it( ch_list.begin()); it != ch_list.end(); ++it)
    {
	std::string name( it.name());

	if( is_rgb_image_)
	{
	    if( name == "R")
		continue;

	    if( name == "G")
		continue;

	    if( name == "B")
		continue;

	    if( name == "A")
		continue;
	}
	else
	{
	    if( is_luma_chroma_image_)
	    {
		if( name == "Y")
		    continue;

		if( name == "RY")
		    continue;

		if( name == "BY")
		    continue;

		if( name == "A")
		    continue;
	    }
	}

	add_channel_name( it.name());
    }

    if( is_rgb_image_ || is_luma_chroma_image_)
	has_extra_channels_ = ( channel_list().size() != 4);
    else
	has_extra_channels_ = true;
}

bool exr_reader_t::is_rgb() const
{
    const Imf::ChannelList& ch_list( header_.channels());
    return ch_list.findChannel( "R") && ch_list.findChannel( "G") && ch_list.findChannel( "B");
}

bool exr_reader_t::is_luma_chroma() const
{
    const Imf::ChannelList& ch_list( header_.channels());
    return ch_list.findChannel( "Y");
}

void exr_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample,
				const boost::tuple<int,int,int,int>& channels) const
{
    // this is to handle luma-chroma image
    if( !has_extra_channels())
    {
	if( subsample != 1 && multilevel_)
	    read_exr_image_proxy( path_, view, crop, subsample);
	else
	    read_exr_image( path_, view, crop, subsample);

	return;
    }

    const char *rchannel = 0;
    int indx = boost::get<0>( channels);

    if( indx < channel_list().size())
	rchannel = channel_list()[indx].c_str();

    const char *gchannel = 0;
    indx = boost::get<1>( channels);

    if( indx < channel_list().size())
	gchannel = channel_list()[indx].c_str();

    const char *bchannel = 0;
    indx = boost::get<2>( channels);

    if( indx < channel_list().size())
	bchannel = channel_list()[indx].c_str();

    const char *achannel = 0;
    indx = boost::get<3>( channels);

    if( indx < channel_list().size())
	achannel = channel_list()[indx].c_str();

    if( subsample != 1 && multilevel_)
	read_exr_image_proxy( path_, view, crop, rchannel, gchannel, bchannel, achannel, subsample);
    else
	read_exr_image( path_, view, crop, rchannel, gchannel, bchannel, achannel, subsample);
}

} // namespace
} // namespace
