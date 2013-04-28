// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image_input/image_input_node.hpp>

#include<stdlib.h>

#include<sstream>

#include<boost/bind.hpp>

#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/imageio/multichannel_reader.hpp>

#include<ramen/nodes/image_input/image_input_param.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/util/path.hpp>

namespace ramen
{

image_input_node_t::image_input_node_t() : image_node_t()
{
    set_name( "image_in");
    add_output_plug();

    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

image_input_node_t::image_input_node_t( const boost::filesystem::path& path, bool sequence) : image_node_t()
{
    set_name( "image_in");
    add_output_plug();
    std::auto_ptr<image_input_param_t> p( new image_input_param_t( "Path", path, sequence));
    p->set_id( "image");
    add_param( p);

    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

image_input_node_t::image_input_node_t( const image_input_node_t& other) : image_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

void image_input_node_t::set_channels( const std::string& red, const std::string& green,
					const std::string& blue, const std::string& alpha)
{
    time_ = 1.0f;
    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
    fname_ = p->path_for_frame( time_);

    if( is_relative( fname_))
    {
	assert( composition());
	fname_ = composition()->relative_to_absolute( fname_);
    }

    reader_.reset();

    try
    {
	reader_ = imageio::factory_t::Instance().reader_for_image( fname_);
    }
    catch( std::exception&)
    {
    }

    p->set_channels( red, green, blue, alpha);
}

void image_input_node_t::param_changed( param_t *p)
{
    if( p == &param( "image"))
	set_frame( time_);
}

void image_input_node_t::do_create_params()
{
    if( param_set().empty())
    {
	std::auto_ptr<image_input_param_t> p( new image_input_param_t( "Path"));
	p->set_id( "image");
	add_param( p);
    }
}

void image_input_node_t::do_set_frame( float f)
{
    time_ = f;
    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
    fname_ = p->path_for_frame( time_);

    if( is_relative( fname_))
    {
	assert( composition());
	fname_ = composition()->relative_to_absolute( fname_);
    }

    reader_.reset();

    try
    {
	reader_ = imageio::factory_t::Instance().reader_for_image( fname_);
    }
    catch( std::exception&)
    {
    }

    p->update_widgets();
}

bool image_input_node_t::do_is_valid() const { return reader_.get();}

void image_input_node_t::do_calc_hash( const render::render_context_t& context)
{ 
    append_to_hash( hash_string( fname_));

    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
    boost::tuple<int,int,int,int> channels = p->channels();

    std::stringstream s;
    s << channels;
    append_to_hash( s.str());
}

void image_input_node_t::do_calc_domain( const render::render_context_t& context)
{ 
    set_domain( reader_->domain());
}

void image_input_node_t::do_calc_bounds( const render::render_context_t& context)
{
    set_bounds( reader_->defined());
}

void image_input_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( intersect( reader_->defined(), interest()));
}

void image_input_node_t::do_subsample_areas( const render::render_context_t& context)
{
    real_defined_ = defined();
    image_node_t::do_subsample_areas( context);
}

void image_input_node_t::do_process( const render::render_context_t& context)
{
    if( real_defined_.isEmpty())
	return;

    try
    {
	imageio::multichannel_reader_t *mreader = dynamic_cast<imageio::multichannel_reader_t*>( reader_.get());

	if( mreader)
	{
	    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
	    boost::tuple<int,int,int,int> channels = p->channels();
	    mreader->read_image( image_view(), real_defined_, context.subsample, channels);
	}
	else
	    reader_->read_image( image_view(), real_defined_, context.subsample);
    }
    catch( ...)
    {
    }
}

void image_input_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void image_input_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

// factory
image_node_t *create_image_input_node() { return new image_input_node_t();}

image_node_t *create_gui_image_input_node()
{
    bool sequence, relative;
    boost::filesystem::path p;

    if( ui::user_interface_t::Instance().image_sequence_file_selector( p, sequence, relative))
        return new image_input_node_t( p, sequence);

    return 0;
}

node_info_t image_input_node_info()
{
    node_info_t info;
    info.id = "builtin.image_input";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Image...";
    info.create = &create_image_input_node;
    info.create_gui = &create_gui_image_input_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( image_input_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::image_input_node_t)
