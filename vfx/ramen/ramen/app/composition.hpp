/**
 * @file
 * @brief composition class
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMPOSITION_HPP
#define	RAMEN_COMPOSITION_HPP

#include<vector>

#include<boost/filesystem/path.hpp>

#include<boost/serialization/access.hpp>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

#include<ramen/dataflow/graph.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/app/unique_name_map.hpp>
#include<ramen/render/render_context.hpp>

#include<ramen/util/format.hpp>

namespace ramen
{

/**
 * @class composition_t
 * @brief composition class
 *
 * A composition_t contains a graph of nodes, a time range and
 * a stack of rendering options.
 *
 */

class composition_t
{
public:

    typedef dataflow::graph<node_t>	graph_type;
    typedef graph_type::node_type	node_type;
    typedef graph_type::edge_type	edge_type;

    typedef graph_type::node_iterator		node_iterator;
    typedef graph_type::const_node_iterator	const_node_iterator;

    typedef graph_type::reverse_node_iterator		reverse_node_iterator;
    typedef graph_type::const_reverse_node_iterator	const_reverse_node_iterator;

    typedef graph_type::node_range_type		node_range_type;
    typedef graph_type::const_node_range_type	const_node_range_type;

    typedef graph_type::edge_iterator		edge_iterator;
    typedef graph_type::const_edge_iterator	const_edge_iterator;

    typedef graph_type::reverse_edge_iterator		reverse_edge_iterator;
    typedef graph_type::const_reverse_edge_iterator	const_reverse_edge_iterator;

    typedef graph_type::edge_range_type		edge_range_type;
    typedef graph_type::const_edge_range_type	const_edge_range_type;

    composition_t();
    composition_t( const composition_t& other);

    /**
     * Adds a node to the composition
     * @param n the node to add
     */
    void add_node( std::auto_ptr<node_type> n);

    /**
     * Removes a node from the composition
     * @param n the node to add
     */
    std::auto_ptr<node_type> release_node( node_type *n);

    node_t *find_node( const std::string& name);

    void add_edge( const edge_type& e, bool notify = false);
    void remove_edge( const edge_type& e, bool notify = false);

    node_range_type& nodes()			{ return g_.nodes();}
    const_node_range_type& nodes() const	{ return g_.nodes();}

    const_edge_range_type& edges() const	{ return g_.edges();}
    edge_range_type& edges()			{ return g_.edges();}

    // connections

    bool can_connect( node_type *src, node_type *dst, int port) const;

    /**
     * Connect two nodes
     * @param src the source node
     * @param dst the destination node
     * @param port the destination port
     */
    void connect( node_type *src, node_type *dst, int port);

    /**
     * Disconnect two nodes
     * @param src the source node
     * @param dst the destination node
     * @param port the destination port
     */
    void disconnect( node_type *src, node_type *dst, int port);

    /**
     * Renames a node.
     * @param n the node to rename
     * @param new_name the new name
     */
    void rename_node( node_type *n, const std::string& new_name);

    int start_frame() const	{ return start_frame_;}
    int end_frame() const	{ return end_frame_;}
    float frame() const		{ return frame_;}

    void set_start_frame( int f) { start_frame_ = f;}
    void set_end_frame( int f)	{ end_frame_ = f;}
    void set_frame( float f);

    int subsample() const	{ return subsample_;}
    void set_subsample( int s);

    /**
     * Return the current render context
     * @param mode the current rendering mode
     */
    render::render_context_t current_context( render::render_mode mode = render::interface_render) const;

    /**
     * Sets the current render context
     * @param context the new context
     * @param recalc_areas recalc the nodes areas (defined, domain and interest)
     */
    void set_current_context( const render::render_context_t& context, bool recalc_areas = true);

    format_t default_format() const             { return default_format_;}
    void set_default_format( const format_t& f) { default_format_ = f;}

    int frame_rate() const        { return frame_rate_;}
    void set_frame_rate( int f)   { frame_rate_ = f;}

    node_t *result_node()		{ return result_node_;}
    void set_result_node( node_t *n)	{ result_node_ = n;}

    const Imath::Box2i& interest() const	   { return interest_;}
    void set_interest( const Imath::Box2i& roi)    { interest_ = roi;}

//    int motion_blur_extra_samples() const       { return motion_blur_extra_samples_;}
//    void set_motion_blur_extra_samples( int s)  { motion_blur_extra_samples_ = s;}
//
//    float motion_blur_shutter_factor() const        { return motion_blur_shutter_factor_;}
//    void set_motion_blur_shutter_factor( float f)   { motion_blur_shutter_factor_ = f;}

    void save_render_state();
    void restore_render_state();

    // home directory, for relative paths
    const boost::filesystem::path& composition_dir() const;
    void set_composition_dir( const boost::filesystem::path& dir);

    boost::filesystem::path relative_to_absolute( const boost::filesystem::path& p) const;
    boost::filesystem::path absolute_to_relative( const boost::filesystem::path& p) const;

    bool playing() const	{ return playing_;}
    void set_playing( bool b)	{ playing_ = b;}

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    boost::filesystem::path composition_dir_;

    dataflow::graph<node_t> g_;
    unique_name_map_t node_map_;
    int start_frame_, end_frame_;
    float frame_;
    int subsample_;
    format_t default_format_;
    int frame_rate_;
    Imath::Box2i interest_;
    node_t *result_node_;

    int motion_blur_extra_samples_;
    float motion_blur_shutter_factor_;

    std::vector<render::render_context_t> render_state_stack_;

    bool playing_;
};

} // namespace

#endif

