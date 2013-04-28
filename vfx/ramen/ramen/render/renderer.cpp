// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.


#include<ramen/render/renderer.hpp>

#include<sstream>

#include<boost/bind.hpp>

#include<ramen/dataflow/algorithm.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<iostream>

namespace
{

std::ostream& print_box( std::ostream& os, const Imath::Box2i& box)
{
    os << "(( " << box.min.x << ", " << box.min.y << "), ( " << box.max.x << ", " << box.max.y << "))";
    return os;
}

struct print_areas
{
    void operator()( ramen::node_t& n) const
    {
	std::cout << n.name() << " ";

	//std::cout << n.hash_str() << "\n";

        // print digest
	//std::cout << "[";
	//for( int i=0;i<16;++i)
	  //  std::cout << (int) n.digest()[i];
	//std::cout << "]\n";

	std::cout << "domain   = ";   print_box( std::cout, n.domain()) << "\n";
	std::cout << "bounds   = ";  print_box( std::cout, n.bounds()) << "\n";
	std::cout << "interest = "; print_box( std::cout, n.interest()) << "\n";
	std::cout << "defined  =";   print_box( std::cout, n.defined()) << "\n\n";
    }
};

} // unnamed

namespace ramen
{
namespace render
{

#ifndef NDEBUG
    bool renderer_t::log_areas = true;
#else
    bool renderer_t::log_areas = false;
#endif

renderer_t::renderer_t( const render_context_t& c, bool save_state, bool release_image) : context_( c)
{
    restore_state_ = save_state;
    release_image_ = release_image;

    if( save_state)
	context_.composition->save_render_state();

    context_.composition->set_current_context( context_, false);

    node_t *n = context_.result_node;
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::calc_domain, _1, context_));
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::calc_bounds, _1, context_));
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::clear_interest, _1));
}

renderer_t::~renderer_t()
{
    if( release_image_)
	context_.composition->result_node()->release_image();

    if( restore_state_)
	context_.composition->restore_render_state();
}

void renderer_t::calc_areas( const Imath::Box2i& roi)
{
    node_t *n = context_.result_node;
    n->set_interest( roi);
    context_.interest = roi;
    context_.composition->set_interest( roi);

    dataflow::breadth_first_inputs_apply( *n, boost::bind( &node_t::calc_inputs_interest, _1, context_));    
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::calc_defined, _1, context_));
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::subsample_areas, _1, context_));
}

const Imath::Box2i& renderer_t::domain() const
{
    return context_.composition->result_node()->domain();
}

const Imath::Box2i& renderer_t::bounds() const
{
    return context_.composition->result_node()->bounds();
}

void renderer_t::render( const Imath::Box2i& roi)
{
    node_t *n = context_.result_node;
    calc_areas( roi);

    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::clear_hash, _1));
    dataflow::depth_first_inputs_search( *n, boost::bind( &node_t::calc_hash, _1, context_));

    if( log_areas)
        dataflow::depth_first_inputs_search( *n, print_areas()); // only while debugging

    n->recursive_process( context_);
}

void renderer_t::render() { render( domain());}

image::image_buffer_t renderer_t::image()
{
    return context_.composition->result_node()->image();
}

image::const_image_view_t renderer_t::domain_image_view() const
{
    return context_.composition->result_node()->const_subimage_view( domain());
}

} // namespace
} // namespace
