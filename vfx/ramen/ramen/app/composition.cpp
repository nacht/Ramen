// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/config.hpp>

#include<ramen/app/composition.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/serialization/nvp.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/ImathExt/serialization/ImathBox.hpp>

#include<ramen/dataflow/algorithm.hpp>
#include<ramen/app/preferences.hpp>
#include<ramen/params/composite_param.hpp>
#include<ramen/util/path.hpp>

#include<ramen/ofx/ofx_node.hpp>

#include<iostream>

namespace ramen
{

namespace
{

struct node_is_connected_to_node
{
    node_is_connected_to_node( composition_t::node_type *n) : n_( n), result( false) {}

    void operator()( const composition_t::node_type& node)
    {
        if( n_ == &node)
            result = true;
    }

    composition_t::node_type *n_;
    bool result;
};

} // unnamed

composition_t::composition_t()
{
    start_frame_ = 1;
    end_frame_ = 100;
    frame_ = 1;
    subsample_ = 1;
    default_format_ = preferences_t::Instance().default_format();
    frame_rate_ = preferences_t::Instance().frame_rate();
    result_node_ = 0;
    motion_blur_extra_samples_ = 0;
    motion_blur_shutter_factor_ = 0;
    playing_ = false;
}

composition_t::composition_t( const composition_t& other) : g_( other.g_)
{
    start_frame_ = other.start_frame_;
    end_frame_ = other.end_frame_;
    frame_ = other.frame_;
    subsample_ = other.subsample_;
    default_format_ = other.default_format_;
    frame_rate_ = other.frame_rate_;
    result_node_ = 0;
    motion_blur_extra_samples_ = other.motion_blur_extra_samples_;
    motion_blur_shutter_factor_ = other.motion_blur_shutter_factor_;
    playing_ = other.playing_;
}

void composition_t::add_node( std::auto_ptr<node_t> n)
{
    node_map_.insert( &(*n));
    n->set_composition( this);
    n->subsample_changed( subsample_);
    n->set_frame( frame());

    render::render_context_t context = current_context();
    context.subsample = 1;
    n->calc_domain( context);
    n->domain_changed();
    g_.add_node( n);
}

std::auto_ptr<node_t> composition_t::release_node( node_t *n)
{
    node_map_.remove( n);
    n->set_composition( 0);
    return g_.release_node( n);
}

node_t *composition_t::find_node( const std::string& name)
{
    return node_map_.find( name);
}

void composition_t::add_edge( const edge_type& e, bool notify)
{
    g_.add_edge( e);

    if( notify)
	e.dst->connected( e.src, e.port);
}

void composition_t::remove_edge( const edge_type& e, bool notify)
{
    g_.remove_edge( e);

    if( notify)
	e.dst->connected( 0, e.port);
}

void composition_t::rename_node( node_t *n, const std::string& new_name)
{
    node_map_.remove( n);
    n->set_name( new_name);
    node_map_.insert( n);
}

bool composition_t::can_connect( node_type *src, node_type *dst, int port) const
{
    node_is_connected_to_node fun( src);
    // it would be so nice if boost::ref defined operator()...
    dataflow::breadth_first_outputs_search( *dst, boost::bind<void>( boost::ref( fun), _1));
    return fun.result == false;
}

void composition_t::connect( node_type *src, node_type *dst, int port)
{
    g_.connect( src, dst, port);
    dst->connected( src, port);
}

void composition_t::disconnect( node_type *src, node_type *dst, int port)
{
    g_.disconnect( src, dst, port);
    dst->connected( 0, port);
}

void composition_t::set_frame( float f)
{
    if( frame_ != f)
    {
	frame_ = f;
	adobe::for_each( nodes(), boost::bind( &node_t::set_frame, _1, f));
    }
}

void composition_t::set_subsample( int s)
{
    if( s != subsample_)
    {
	subsample_ = s;
	adobe::for_each( nodes(), boost::bind( &node_t::subsample_changed, _1, s));
    }
}

render::render_context_t composition_t::current_context( render::render_mode mode) const
{
    render::render_context_t c;
    c.mode = mode;
    c.composition = const_cast<composition_t*>( this);
    c.result_node = result_node_;
    c.subsample = subsample_;
    c.time = frame_;
    c.default_format = default_format_;
    c.interest = interest_;
    c.motion_blur_extra_samples = motion_blur_extra_samples_;
    c.motion_blur_shutter_factor = motion_blur_shutter_factor_;
    return c;
}

void composition_t::set_current_context( const render::render_context_t& context, bool recalc_areas)
{
    set_subsample( context.subsample);
    default_format_ = context.default_format;
    interest_ = context.interest;
    result_node_ = context.result_node;
    motion_blur_extra_samples_ = context.motion_blur_extra_samples;
    motion_blur_shutter_factor_ = context.motion_blur_shutter_factor;

    set_frame( context.time);

    if( result_node_ && recalc_areas)
    {
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::calc_domain, _1, context));
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::calc_bounds, _1, context));
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::clear_interest, _1));

	if( context.interest.isEmpty())
	    result_node_->set_interest( result_node_->domain());
	else
	    result_node_->set_interest( context.interest);

	dataflow::breadth_first_inputs_apply( *result_node(), boost::bind( &node_t::calc_inputs_interest, _1, context));
	
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::calc_defined, _1, context));
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::subsample_areas, _1, context));

	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::clear_hash, _1));
	dataflow::depth_first_inputs_search( *result_node(), boost::bind( &node_t::calc_hash, _1, context));
    }
}

void composition_t::save_render_state() { render_state_stack_.push_back( current_context());}

void composition_t::restore_render_state()
{
    assert( !render_state_stack_.empty());
    render::render_context_t c( render_state_stack_.back());
    render_state_stack_.pop_back();
    set_current_context( c);
}

const boost::filesystem::path& composition_t::composition_dir() const { return composition_dir_;}

void composition_t::set_composition_dir( const boost::filesystem::path& dir) { composition_dir_ = dir;}

boost::filesystem::path composition_t::relative_to_absolute( const boost::filesystem::path& p) const
{
    assert( !composition_dir_.empty());
    return make_absolute_path( p, composition_dir());
}

boost::filesystem::path composition_t::absolute_to_relative( const boost::filesystem::path& p) const
{
    assert( !composition_dir_.empty());
    return make_relative_path( p, composition_dir());
}

void composition_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    // BOOST_CLASS_EXPORT is not working for composite param_t for some reason, we register it manually.
    ar.register_type( static_cast< ramen::composite_param_t*>( NULL));

    int app_version;
    ar & boost::serialization::make_nvp( "app_version", app_version);

    // do something with app version here

    ar & boost::serialization::make_nvp( "graph", g_);
    BOOST_FOREACH( node_t& n, nodes()) { node_map_.insert( &n);}

    ar & boost::serialization::make_nvp( "start_frame", start_frame_);
    ar & boost::serialization::make_nvp( "end_frame", end_frame_);
    ar & boost::serialization::make_nvp( "frame", frame_);
    ar & boost::serialization::make_nvp( "subsample", subsample_);
    ar & boost::serialization::make_nvp( "default_format", default_format_);
    ar & boost::serialization::make_nvp( "frame_rate", frame_rate_);
    ar & boost::serialization::make_nvp( "interest", interest_);

    adobe::for_each( nodes(), boost::bind( &node_t::set_composition, _1, this));

    // this can only be done after setting the composition pointer
    BOOST_FOREACH( node_t& n, nodes())
    {
	ofx_node_t *ofx_node = dynamic_cast<ofx_node_t*>( &n);
	
	if( ofx_node)
	    ofx_node->create_effect_instance( false);
    }

    BOOST_FOREACH( edge_type& e, edges())
    {
	e.dst->connected( e.src, e.port);
    }

    adobe::for_each( nodes(), boost::bind( &node_t::subsample_changed, _1, subsample_));
    adobe::for_each( nodes(), boost::bind( &node_t::set_frame, _1, frame_));

    render::render_context_t context = current_context();
    context.subsample = 1;

    BOOST_FOREACH( node_t& n, nodes())
    {
	if( n.num_outputs() == 0)
	    n.recursive_calc_domain( context);
    }

    adobe::for_each( nodes(), boost::bind( &manipulable_t::create_manipulators, _1));
    adobe::for_each( nodes(), boost::bind( &node_t::domain_changed, _1));
}

void composition_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    // BOOST_CLASS_EXPORT is not working for composite param_t for some reason
    // so we register it manually
    ar.register_type( static_cast< ramen::composite_param_t*>( NULL));

    int app_version = RAMEN_VERSION;
    ar & boost::serialization::make_nvp( "app_version", app_version);

    ar & boost::serialization::make_nvp( "graph", g_);
    ar & boost::serialization::make_nvp( "start_frame", start_frame_);
    ar & boost::serialization::make_nvp( "end_frame", end_frame_);
    ar & boost::serialization::make_nvp( "frame", frame_);
    ar & boost::serialization::make_nvp( "subsample", subsample_);
    ar & boost::serialization::make_nvp( "default_format", default_format_);
    ar & boost::serialization::make_nvp( "frame_rate", frame_rate_);
    ar & boost::serialization::make_nvp( "interest", interest_);
}

} // namespace
