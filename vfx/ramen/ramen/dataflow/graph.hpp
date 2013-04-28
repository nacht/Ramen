// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DATAFLOW_GRAPH_HPP
#define RAMEN_DATAFLOW_GRAPH_HPP

#include<algorithm>
#include<memory>
#include<map>

#include<boost/bind.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/split_member.hpp>
#include<boost/serialization/vector.hpp>
#include<boost/ptr_container/serialize_ptr_vector.hpp>

#include<ramen/dataflow/edge.hpp>

/**
 * dataflow: a more or less generic dataflow layer
 */

namespace ramen
{
namespace dataflow
{

template<class Node>
class graph
{
public:

	typedef Node node_type;
	typedef edge_t<Node> edge_type;

	typedef typename boost::ptr_vector<node_type>::iterator		node_iterator;
	typedef typename boost::ptr_vector<node_type>::const_iterator	const_node_iterator;

	typedef typename boost::ptr_vector<node_type>::reverse_iterator		reverse_node_iterator;
	typedef typename boost::ptr_vector<node_type>::const_reverse_iterator	const_reverse_node_iterator;

	typedef boost::ptr_vector<node_type>		node_range_type;
	typedef const boost::ptr_vector<node_type>	const_node_range_type;
	
	typedef typename std::vector<edge_type>::iterator	edge_iterator;
	typedef typename std::vector<edge_type>::const_iterator const_edge_iterator;

	typedef typename std::vector<edge_type>::reverse_iterator	reverse_edge_iterator;
	typedef typename std::vector<edge_type>::const_reverse_iterator const_reverse_edge_iterator;

	typedef std::vector<edge_type>			edge_range_type;
	typedef const std::vector<edge_type>	const_edge_range_type;

	graph() {}
	~graph() {}

	graph( const graph& other)
	{
		std::map<const node_type*, node_type*> relation;
		
		for( const_node_iterator it( other.nodes().begin()); it != other.nodes().end(); ++it)
		{
			node_type *n = new_clone( *it);
			relation[&(*it)] = n;
			nodes().push_back( n);
		}
		
		for( const_edge_iterator it( other.edges().begin()); it != other.edges().end(); ++it)
			add_edge( edge_type( relation[it->src], relation[it->dst], it->port));
	}

	void swap( graph& other)
	{
		nodes_.swap( other.nodes_);
		edges_.swap( other.edges_);
	}

	void add_node( std::auto_ptr<Node> n) { nodes_.push_back( n);}

	std::auto_ptr<Node> release_node( Node *n)
	{
		typename boost::ptr_vector<node_type>::iterator it;
	
		for( it = nodes_.begin(); it != nodes_.end(); ++it)
		{
			if( &(*it) == n)
				break;
		}

		return std::auto_ptr<Node> ( nodes_.release( it).release());
	}
	
	void add_edge( const edge_type& e)
	{
	    node_type *src = e.dst->input_plugs()[e.port].input();
	
	    if( src)
		remove_edge( edge_type( src, e.dst, e.port));

	    e.dst->input_plugs()[e.port].set_input( e.src);
	    e.src->output_plug().add_output( e.dst, e.port);
	    edges_.push_back( e);
	}
	
	void remove_edge( const edge_type& e)
	{
		e.src->output_plug().remove_output( e.dst, e.port);
		e.dst->input_plugs()[e.port].clear_input();
		edges_.erase( std::find( edges_.begin(), edges_.end(), e));
	}

	node_iterator nodes_begin() { return nodes_.begin();}
	node_iterator nodes_end() { return nodes_.end();}

	const_node_iterator nodes_begin() const	{ return nodes_.begin();}
	const_node_iterator nodes_end() const		{ return nodes_.end();}

	node_range_type& nodes()				{ return nodes_;}
	const_node_range_type& nodes() const	{ return nodes_;}

	edge_iterator edges_begin() { return edges_.begin();}
	edge_iterator edges_end() { return edges_.end();}

	const_edge_iterator edges_begin() const	{ return edges_.begin();}
	const_edge_iterator edges_end() const		{ return edges_.end();}

	const_edge_range_type& edges() const	{ return edges_;}
	edge_range_type& edges()				{ return edges_;}

	// connections
	void connect( Node *src, Node *dst, int port)		{ add_edge( edge_type( src, dst, port));}
	void disconnect( Node *src, Node *dst, int port)	{ remove_edge( edge_type( src, dst, port));}

private:

	friend class boost::serialization::access;

    template<class Archive>
    void save( Archive & ar, const unsigned int version) const
	{
		ar & boost::serialization::make_nvp( "nodes", nodes_);
		ar & boost::serialization::make_nvp( "edges", edges_);
	}

    template<class Archive>
    void load( Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp( "nodes", nodes_);
		
		std::vector<edge_type> e;
		ar & boost::serialization::make_nvp( "edges", e);
		std::for_each( e.begin(), e.end(), boost::bind( &graph<Node>::add_edge, this, _1));
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	boost::ptr_vector<node_type> nodes_;
	std::vector<edge_type> edges_;
};

} // namespace
} // namespace

#endif
