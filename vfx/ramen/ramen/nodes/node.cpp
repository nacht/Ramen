// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node.hpp>

#include<stdexcept>
#include<algorithm>

#include<boost/bind.hpp>
#include<boost/serialization/string.hpp>
#include<boost/serialization/vector.hpp>
#include<boost/ptr_container/serialize_ptr_vector.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/serialization/ImathBox.hpp>

#include<ramen/dataflow/algorithm.hpp>

#include<ramen/image/color_bars.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/image/image_cache.hpp>

#include<ramen/nodes/reset_node_command.hpp>

#include<iostream>

namespace ramen
{

node_t::node_t() : flags_(0) {}

node_t::node_t( const node_t& other) : dataflow::base_node_t<node_t>( other), manipulable_t( other), name_( other.name_), flags_( other.flags_),
					loc_( other.loc_),plugs_info_( other.plugs_info_)
{
}

node_t::~node_t() {}

bool node_t::selected() const	{ return flags_ & selected_bit;}

void node_t::select( bool state)
{
    if( state)
	flags_ |= selected_bit;
    else
	flags_ &= ~selected_bit;
}

void node_t::toggle_selection()	{ select( !selected());}

bool node_t::ignored() const	{ return flags_ & ignored_bit;}

void node_t::set_ignored( bool b)
{
    if( b)
	flags_ |= ignored_bit;
    else
	flags_ &= ~ignored_bit;
}

bool node_t::plugin_error() const
{
    return flags_ & plugin_error_bit;
}

void node_t::set_plugin_error( bool b)
{
    if( b)
	flags_ |= plugin_error_bit;
    else
	flags_ &= ~plugin_error_bit;
}

bool node_t::can_motion_blur() const
{
    return flags_ & can_motion_blur_bit;
}

void node_t::set_can_motion_blur( bool b)
{
    if( b)
	flags_ |= can_motion_blur_bit;
    else
	flags_ &= ~can_motion_blur_bit;
}

bool node_t::cacheable() const	{ return flags_ & cacheable_bit;}

void node_t::set_cacheable( bool b)
{
    if( b)
	flags_ |= cacheable_bit;
    else
	flags_ &= ~cacheable_bit;
}

bool node_t::is_active() const     { return flags_ & active_bit;}
bool node_t::is_context() const    { return flags_ & context_bit;}

bool node_t::do_accept_input( node_t *src, int port) const { return true;}
void node_t::do_connected( node_t *src, int port) {}
void node_t::do_input_changed( node_t *input) {}

void node_t::add_input_plug( const input_plug_info_t& info, bool optional)
{
    plugs_info_.push_back( info);
    dataflow::base_node_t<node_t>::add_input_plug( optional);
}

void node_t::set_plug_info( std::size_t index, const input_plug_info_t& info) { plugs_info_[index] = info;}

// params
void node_t::param_edit_finished()
{
    notify();
    notify_dependents();
}

void node_t::domain_changed()
{
    adobe::for_each( param_set(), boost::bind( &param_t::domain_changed, _1, domain()));
}

void node_t::notify()
{
    // keep the domain up to date
    Imath::Box2i old_domain( domain());

    render::render_context_t context = composition()->current_context();
    context.subsample = 1;
    calc_domain( context);

    if( old_domain != domain())
	domain_changed();
    
    changed( this);
}

void node_t::notify_dependents()
{
    for( int i = 0; i < num_outputs(); ++i)
	output( i)->input_changed( this);

    dataflow::breadth_first_outputs_search( *this, boost::bind( &node_t::notify, _1), false);
}

void node_t::set_frame( float f)
{
    parameterised_t::evaluate( f);
    do_set_frame( f);
}

void node_t::begin_edit()
{
    flags_ |= active_bit;
    do_begin_edit();
}

void node_t::end_edit()
{
    do_end_edit();
    flags_ &= ~active_bit;
}

void node_t::begin_context()
{
    flags_ |= context_bit;
    do_begin_context();
}

void node_t::end_context()
{
    do_end_context();
    flags_ &= ~context_bit;
}

bool node_t::is_valid() const
{
    BOOST_FOREACH( const input_plug_type& i, input_plugs())
    {
	if( !i.connected() && !i.optional())
	    return false;
    }

    if( ignored())
    {
        if( num_inputs() == 0)
            return false;

        // handle the all inputs optional case
        bool all_optional = true;
        bool all_disconnected = true;

        BOOST_FOREACH( const input_plug_type& i, input_plugs())
        {
            if( i.connected())
                all_disconnected = false;
                
            if( !i.optional())
                all_optional = false;
        }
        
        if( all_optional && all_disconnected)
            return false;
    }

    return do_is_valid();
}

bool node_t::do_is_valid() const { return true;}

// undo
std::auto_ptr<undo::command_t> node_t::create_reset_command()
{
    std::auto_ptr<undo::reset_node_command_t> cmd( new undo::reset_node_command_t( this));

    std::auto_ptr<undo::command_t> c( param_set().create_reset_command());

    if( c.get())
        cmd->push_back( c);

    c = do_create_reset_command();

    if( c.get())
        cmd->push_back( c);

    if( cmd->empty())
        return std::auto_ptr<undo::command_t>();
    else
        return std::auto_ptr<undo::command_t>( cmd.release());
}

std::auto_ptr<undo::command_t> node_t::do_create_reset_command() { return std::auto_ptr<undo::command_t>();}

void node_t::set_domain( const Imath::Box2i& d) { domain_ = d;}

void node_t::calc_domain( const render::render_context_t& context)
{
    is_valid_ = is_valid();

    if( is_valid_ && !ignored())
    {
	do_calc_domain( context);
	return;
    }

    if( ignored() && ( num_inputs() != 0) && input())
    {
	set_domain( input()->domain());
	return;
    }

    set_domain( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( context.default_format.area().max.x - 1, context.default_format.area().max.y - 1)));
}

void node_t::do_calc_domain( const render::render_context_t& context)
{
    if( num_inputs() != 0)
    {
        if( input())
        {
	    set_domain( input()->domain());
	    return;
        }
    }

    // as a fallback set the domain to the default
    set_domain( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( context.default_format.area().max.x - 1, context.default_format.area().max.y - 1)));
}

void node_t::set_bounds( const Imath::Box2i& bounds) { bounds_ = bounds;}

void node_t::calc_bounds( const render::render_context_t& context)
{
    if( is_valid_ && !ignored())
    {
	do_calc_bounds( context);
	return;
    }

    if( ignored() && ( num_inputs() != 0) && input())
    {
	set_bounds( input()->bounds());
	return;
    }

    set_bounds( domain());
}

void node_t::do_calc_bounds( const render::render_context_t& context)
{
    if( num_inputs() != 0)
    {
        if( input())
        {
	    set_bounds( input()->bounds());
	    return;
        }
    }

    // as a fallback set the domain to the default
    set_bounds( domain());
}

void node_t::clear_interest() { interest_ = Imath::Box2i();}

void node_t::set_interest( const Imath::Box2i& roi) { interest_ = roi;}
void node_t::add_interest( const Imath::Box2i& roi) { interest_.extendBy( roi);}

void node_t::calc_inputs_interest( const render::render_context_t& context)
{
    if( is_valid_ && !ignored())
	do_calc_inputs_interest( context);
    else
    {
	// just pass the interest area to the inputs
        BOOST_FOREACH( input_plug_type& i, input_plugs())
	{
	    if( i.connected())
		i.input()->add_interest( interest_);
	}
    }
}

void node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    // by default, pass the interest area to the inputs
    BOOST_FOREACH( input_plug_type& i, input_plugs())
    {
        if( i.connected())
            i.input()->add_interest( interest_);
    }
}

void node_t::set_defined( const Imath::Box2i& b) { defined_ = b;}

void node_t::calc_defined( const render::render_context_t& context)
{
    // TODO: this could be more elegant
    if( ignored())
    {
	if( num_inputs() != 0 && input())
	    set_defined( input()->defined());
	else
	    defined_ = intersect( domain_, interest_);

	return;
    }

    if( is_valid_)
    {
        do_calc_defined( context);
	return;
    }

    defined_ = intersect( domain_, interest_);
}

void node_t::do_calc_defined( const render::render_context_t& context)
{
    defined_ = intersect( domain_, interest_);
}

void node_t::subsample_areas( const render::render_context_t& context)
{
    do_subsample_areas( context);
}

void node_t::do_subsample_areas( const render::render_context_t& context)
{
    if( context.subsample != 1)
    {
        domain_   = scale( domain_  , 1.0f / context.subsample);
	// TODO: maybe, subsample bounds here?
        interest_ = scale( interest_, 1.0f / context.subsample);

	if( !defined().isEmpty())
	    defined_  = scale( defined_ , 1.0f / context.subsample);
    }
}

void node_t::recursive_calc_domain( const render::render_context_t& context)
{
    dataflow::depth_first_inputs_search( *this, boost::bind( &node_t::calc_domain, _1, context));
    domain_ = scale( domain_  , 1.0f / context.subsample);
}

void node_t::recursive_calc_domain()
{
    render::render_context_t context = composition()->current_context();
    recursive_calc_domain( context);
}

void node_t::clear_hash()
{ 
    hash_generator().reset();
    set_cacheable( !is_frame_varying());

    if( cacheable())
    {
        BOOST_FOREACH( input_plug_type& i, input_plugs())
        {
            if( i.connected())
            {
                if( !i.input()->cacheable())
                {
                    set_cacheable( false);
                    return;
                }
            }
        }
    }
}

void node_t::calc_hash( const render::render_context_t& context)
{
    if( !cacheable())
        return;

    hash_generator() << typeid( *this).name();

    char subsample_str[2];
    subsample_str[0] = context.subsample + '0';
    subsample_str[1] = 0;

    if( !is_valid())
    {
	hash_generator() << subsample_str;
	return;
    }

    if( ignored())
	hash_generator() << "1";
    else
	hash_generator() << "0";

    if( num_inputs() == 0)
	hash_generator() << subsample_str;
    else
    {
	// handle the case when all inputs are optional
	bool all_optional = true;
	bool not_connected = true;

	for( int i = 0; i < num_inputs(); ++i)
	{
	    if( !input_plugs()[i].optional())
		all_optional = false;

	    if( input(i))
		not_connected = false;
	}

	if( all_optional && not_connected)
	    hash_generator() << subsample_str;
	else
	{
	    // normal case, accum all inputs
	    int input_num = 0;
	    BOOST_FOREACH( const input_plug_type& i, input_plugs())
	    {
		if( i.connected() && include_input_in_hash( input_num))
		    hash_generator() << i.input()->hash_str();
                else
                    hash_generator() << "0"; // unconnected input

		++input_num;
	    }
	}
    }

    if( can_motion_blur())
    {
        hash_generator() << context.motion_blur_extra_samples;
        hash_generator() << context.motion_blur_shutter_factor;
    }

    hash_generator() << param_set().str();
    do_calc_hash( context);
}

void node_t::append_to_hash( const std::string& str) { hash_generator() << str;}

std::string node_t::hash_str() const { return hash_generator().str();}

const adobe::md5_t::digest_t& node_t::digest() { return hash_generator().digest();}

// cache
bool node_t::is_frame_varying() const   { return false;}
bool node_t::use_cache() const          { return true;}

// cache

bool node_t::read_image_from_cache()
{
    if( !cacheable() || !use_cache())
        return false;

    return do_read_image_from_cache();
}

bool node_t::do_read_image_from_cache()
{
    boost::optional<image::image_buffer_t> cached( image::image_cache_t::Instance().find( digest(), defined()));

    if( cached)
    {
	image_ = cached.get();
	return true;
    }

    return false;
}

void node_t::write_image_to_cache()
{
    if( !cacheable() || !use_cache())
        return;

    if( !defined().isEmpty())
        do_write_image_to_cache();
}

void node_t::do_write_image_to_cache()
{
    image::image_cache_t::Instance().insert( digest(), image_);
}

// images
void node_t::alloc_image()
{ 
    image_ = image::image_buffer_t( defined());

    if( !image_.empty())
	boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 0));
}

void node_t::release_image()	{ image_ = image::image_buffer_t();}

image::image_view_t node_t::image_view() { return image_.rgba_subimage_view( defined());}

image::const_image_view_t node_t::const_image_view() const { return image_.const_rgba_subimage_view( defined());}

image::image_view_t node_t::subimage_view( int x, int y, int w, int h)
{
    return image_.rgba_subimage_view( Imath::Box2i( Imath::V2i( x, y), Imath::V2i( w - 1, h - 1)));
}

image::image_view_t node_t::subimage_view( const Imath::Box2i& area)
{
    return image_.rgba_subimage_view( area);
}

image::const_image_view_t node_t::const_subimage_view( int x, int y, int w, int h) const
{
    return image_.const_rgba_subimage_view( Imath::Box2i( Imath::V2i( x, y), Imath::V2i( w - 1, h - 1)));
}

image::const_image_view_t node_t::const_subimage_view( const Imath::Box2i& area) const
{
    return image_.const_rgba_subimage_view( area);
}

// processing

void node_t::recursive_process( const render::render_context_t& context)
{
    if( !is_valid())
    {
	alloc_image();
	image::make_color_bars( image_view());
        return;
    }

    if( ignored())
    {
	// TODO: this is not really correct. fix it.
	input()->recursive_process( context);
	image_ = input()->image_;
	input()->release_image();
	return;
    }

    if( read_image_from_cache())
	return;

    do_recursive_process( context);

    write_image_to_cache();

    BOOST_FOREACH( const input_plug_type& i, input_plugs())
    {
        if( i.connected())
            i.input()->release_image();
    }
}

void node_t::do_recursive_process( const render::render_context_t& context)
{
    BOOST_FOREACH( const input_plug_type& i, input_plugs())
    {
        if( i.connected())
            i.input()->recursive_process( context);
    }

    alloc_image();

    // sometimes we need to render again some inputs.
    // the images should be in the cache, unless we
    // are very low on memory.
    // looks a bit like a hack...
    while( 1)
    {
	bool all_inputs_ready = true;

	BOOST_FOREACH( const input_plug_type& i, input_plugs())
	{
	    if( i.connected())
	    {
		if( i.input()->image_empty())
		{
		    i.input()->recursive_process( context);
		    all_inputs_ready = false;
		}
	    }
	}
	
	if( all_inputs_ready)
	    break;
    }

    process( context);
}

void node_t::process( const render::render_context_t& context)
{
    if( !image_empty())
	do_process( context);
}

void node_t::do_process( const render::render_context_t& context) {}

// anim
void node_t::create_tracks( anim::track_t *root)
{
    std::auto_ptr<anim::track_t> top( new anim::track_t( name()));
    do_create_tracks( top.get());
    root->add_child( top);
}

void node_t::do_create_tracks( anim::track_t *parent)
{
    BOOST_FOREACH( param_t& p, param_set()) { p.create_tracks( parent);}
}

// ui
void node_t::draw( ui::image_view_t& view) const    { do_draw( view);}
void node_t::do_draw( ui::image_view_t& view) const {}

void node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( dataflow_base_node_t);
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( manipulable_t);

    ar & boost::serialization::make_nvp( "name", name_);
    ar & boost::serialization::make_nvp( "flags", flags_);
    ar & boost::serialization::make_nvp( "loc", loc_);
    ar & boost::serialization::make_nvp( "plugs_info", plugs_info_);

    // some flags shouldn't be saved with the node state,
    // but now it's late to fix it. So we reset them here.
    flags_ &= ~( selected_bit | cacheable_bit | active_bit | context_bit | plugin_error_bit);
}

void node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( dataflow_base_node_t);
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( manipulable_t);

    ar & boost::serialization::make_nvp( "name", name_);
    ar & boost::serialization::make_nvp( "flags", flags_);
    ar & boost::serialization::make_nvp( "loc", loc_);
    ar & boost::serialization::make_nvp( "plugs_info", plugs_info_);
}

node_t* new_clone( const node_t& other)
{
    node_t *n = other.clone();
    
    for( int i = 0; i < n->num_inputs(); ++i)
	n->connected( 0, i);

    n->create_manipulators();
    n->domain_changed();

    return n;
}

} // namespace

BOOST_CLASS_EXPORT( ramen::node_t::dataflow_base_node_t)
BOOST_CLASS_EXPORT( ramen::node_t)
