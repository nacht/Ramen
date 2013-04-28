// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DATAFLOW_ALGORITHM_HPP
#define RAMEN_DATAFLOW_ALGORITHM_HPP

#include<boost/range.hpp>

#include<ramen/dataflow/base_node.hpp>

namespace ramen
{
namespace dataflow
{

namespace detail
{

template<class Node>
void set_inputs_color( Node& n, color_t c)
{
	n.set_color( c);
	
	for( unsigned int i=0;i<n.num_inputs();++i)
	{
		if( n.input(i) != 0)
			set_inputs_color( *n.input(i), c);
	}
}

template<class Range>
void set_multiple_inputs_color( Range& r, color_t c)
{
    typedef typename boost::range_iterator<Range>::type iter;

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
	set_inputs_color( *(*it), c);
}

template<class Node>
void set_outputs_color( Node& n, color_t c)
{
	n.set_color( c);
	
	for( unsigned int i=0;i<n.num_outputs();++i)
		set_outputs_color( *n.output(i), c);
}

template<class Range>
void set_multiple_outputs_color( Range& r, color_t c)
{
    typedef typename boost::range_iterator<Range>::type iter;

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
	set_outputs_color( *(*it), c);
}

template<class Node, class Visitor>
void depth_first_inputs_recursive_search( Node& n, Visitor f)
{
	for( unsigned int i=0;i<n.num_inputs();++i)
	{
		if( n.input(i) != 0)
			depth_first_inputs_recursive_search( *n.input(i), f);
	}
	
	if( n.color() == black)
	{
		f( n);
		n.set_color( white);
	}
}

template<class Node, class Visitor>
void breadth_first_inputs_recursive_search( Node& n, Visitor f)
{
	if( n.color() == black)
	{
		f( n);
		n.set_color( white);
	}

	for( unsigned int i=0;i<n.num_inputs();++i)
	{
		if( n.input(i) != 0)
			breadth_first_inputs_recursive_search( *n.input(i), f);
	}
}

template<class Node, class Visitor>
void depth_first_outputs_recursive_search( Node& n, Visitor f)
{
	for( unsigned int i=0;i<n.num_outputs();++i)
		depth_first_outputs_recursive_search( *n.output(i), f);
	
	if( n.color() == black)
	{
		f( n);
		n.set_color( white);
	}
}

template<class Node, class Visitor>
void breadth_first_outputs_recursive_search( Node& n, Visitor f)
{
	if( n.color() == black)
	{
		f( n);
		n.set_color( white);
	}

	for( unsigned int i=0;i<n.num_outputs();++i)
		breadth_first_outputs_recursive_search( *n.output(i), f);
}

} // detail

template<class Node, class Visitor>
void breadth_first_inputs_search( Node& first, Visitor f, bool search_first = true)
{
	detail::set_inputs_color( first, black);

	if( !search_first)
	    first.set_color( white);

	detail::breadth_first_inputs_recursive_search( first, f);
}

template<class Node, class Visitor>
void breadth_first_inputs_apply( Node& n, Visitor f, bool apply_first = true)
{
    if( apply_first)
        f( n);

    for( unsigned int i=0;i<n.num_inputs();++i)
    {
	if( n.input(i) != 0)
		breadth_first_inputs_apply( *n.input(i), f);
    }
}

template<class Node, class Visitor>
void depth_first_inputs_search( Node& first, Visitor f, bool search_first = true)
{
	detail::set_inputs_color( first, black);

	if( !search_first)
	    first.set_color( white);

	detail::depth_first_inputs_recursive_search( first, f);
}

template<class Node, class Visitor>
void breadth_first_outputs_search( Node& first, Visitor f, bool search_first = true)
{
	detail::set_outputs_color( first, black);

	if( !search_first)
	    first.set_color( white);

	detail::breadth_first_outputs_recursive_search( first, f);
}

template<class Range, class Visitor>
void breadth_first_multiple_outputs_search( Range& r, Visitor f, bool search_first = true)
{
    typedef typename boost::range_iterator<Range>::type iter;

    detail::set_multiple_outputs_color( r, black);

    if( !search_first)
    {
        for( iter it( boost::begin(r)); it != boost::end(r); ++it)
            (*it)->set_color( white);
    }

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
        detail::breadth_first_outputs_recursive_search( *(*it), f);
}

template<class Node, class Visitor>
void depth_first_outputs_search( Node& first, Visitor f, bool search_first = true)
{
	detail::set_outputs_color( first, black);

	if( !search_first)
	    first.set_color( white);

        detail::depth_first_outputs_recursive_search( first, f);
}

} // namespace
} // namespace

#endif
