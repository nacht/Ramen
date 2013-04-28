// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DATAFLOW_PLUG_HPP
#define RAMEN_DATAFLOW_PLUG_HPP

#include<utility>
#include<vector>
#include<algorithm>

#include<boost/cstdint.hpp>

namespace ramen
{
namespace dataflow
{

template<class Node>
class input_plug_t
{
public:

	input_plug_t( bool optional = false, boost::uint64_t type_mask = 1) : optional_( optional), node_(0)
        {
            set_input_types( type_mask);
        }

	input_plug_t( const input_plug_t& other) : optional_( other.optional_), node_(0)
        {
            set_input_types( other.input_types());
        }

	bool optional() const { return optional_;}

        boost::uint64_t input_types() const { return input_types_;}

        void set_input_types( boost::uint64_t type_mask)
        {
            assert( type_mask != 0);
            // TODO: assert only one bit is set in type_mask
            input_types_ = type_mask;
        }

	bool connected() const	{ return node_ != 0;}
	Node *input() const	{ return node_;}

	void set_input( Node *n) { node_ = n;}
	void clear_input()	 { node_ = 0;}

private:

	Node *node_;
	bool optional_;
        boost::uint64_t input_types_;
};

template<class Node>
class output_plug_t
{
public:

	typedef std::pair<Node*,int> value_type;

	output_plug_t( boost::uint64_t type_mask = 1)
        {
            set_output_type( type_mask);
        }

	output_plug_t( const output_plug_t& other)
        {
            set_output_type( other.output_type());
        }

        boost::uint64_t output_type() const { return output_type_;}

        void set_output_type( boost::uint64_t type_mask)
        {
            assert( type_mask != 0);
            output_type_ = type_mask;
        }

	void add_output( Node *n, int port)
	{
		assert( std::find( nodes_.begin(), nodes_.end(), std::make_pair( n, port)) == nodes_.end());
		nodes_.push_back( std::make_pair( n, port));
	}
	
	void remove_output( Node *n, int port)
	{
		typename std::vector<std::pair<Node*, int> >::iterator it( std::find( nodes_.begin(), nodes_.end(), std::make_pair( n, port)));

		if( it != nodes_.end())
			nodes_.erase( it);
	}

	void clear();
	bool empty() const	{ return nodes_.empty();}

	const value_type& operator[]( int ix) const
	{
		assert( ix < nodes_.size());
		return nodes_[ix];
	}

	const std::vector<value_type>& nodes() const	{ return nodes_;}
	std::vector<value_type>& nodes()		{ return nodes_;}

private:

	std::vector<value_type> nodes_;
        boost::uint64_t output_type_;
};

} // namespace
} // namespace

#endif
