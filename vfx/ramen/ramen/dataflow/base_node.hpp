// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DATAFLOW_BASE_NODE_HPP
#define RAMEN_DATAFLOW_BASE_NODE_HPP

#include<boost/noncopyable.hpp>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/assume_abstract.hpp>

#include<ramen/dataflow/plug.hpp>

namespace ramen
{
namespace dataflow
{

template<class Node> class graph;

enum color_t
{
	black,
	white,
	gray
};

template<class Derived>
class base_node_t
{
public:

	typedef Derived node_type;

	typedef input_plug_t<node_type> input_plug_type;
	typedef output_plug_t<node_type> output_plug_type;

	base_node_t() : output_(0) {}
	virtual ~base_node_t() { delete output_;}

	base_node_t *clone() const { return do_clone();}

	// input
	std::size_t num_inputs() const { return inputs_.size();}

	const std::vector<input_plug_type>& input_plugs() const { return inputs_;}
	std::vector<input_plug_type>& input_plugs()		{ return inputs_;}

	const node_type *input( std::size_t i = 0) const
	{
		assert( i < inputs_.size());
		return inputs_[i].input();
	}

	node_type *input( std::size_t i = 0)
	{
		assert( i < inputs_.size());
		return inputs_[i].input();
	}

        template<class T>
	const T *input_as( std::size_t i = 0) const { return dynamic_cast<const T*>( input( i));}

        template<class T>
	T *input_as( std::size_t i = 0) { return dynamic_cast<T*>( input( i));}

	void add_input_plug( bool optional = false) { inputs_.push_back( input_plug_type( optional));}

	// outputs

	bool has_output_plug() const { return output_ != 0;}

	std::size_t num_outputs() const
	{
		if( !has_output_plug())
			return 0;
		
		return output_->nodes().size();
	}

	const output_plug_type& output_plug() const
	{
		assert( output_);
		return *output_;
	}

	output_plug_type& output_plug()
	{
		assert( output_);
		return *output_;
	}
	
	node_type *output( std::size_t i)
	{
		assert( output_);
		assert( i < num_outputs());
		return output_->nodes()[i].first;
	}

	void add_output_plug()
	{
		assert( !output_);
		output_ = new output_plug_type();
	}

	color_t color() const		{ return color_;}
	void set_color( color_t c)	{ color_ = c;}

protected:

	base_node_t( const base_node_t& other) : inputs_(other.inputs_), output_(0)
	{
	    if( other.output_)
		add_output_plug();
	}
	
	void operator=( const base_node_t&);
	
	std::vector<input_plug_type> inputs_;
	output_plug_type *output_;

private:

	virtual base_node_t *do_clone() const = 0;

	friend class graph<Derived>;

	friend class boost::serialization::access;

        void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version) {}
        void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) {}

	color_t color_;
};

template<class Derived>
inline base_node_t<Derived> *new_clone( const base_node_t<Derived>& other) { return other.clone();}

} // namespace
} // namespace

#endif
