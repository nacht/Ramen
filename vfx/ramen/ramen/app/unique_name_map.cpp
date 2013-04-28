// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/unique_name_map.hpp>

#include<boost/lexical_cast.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{

void unique_name_map_t::swap( unique_name_map_t& other) { map_.swap( other.map_);}

void unique_name_map_t::insert( node_t *n)
{
	std::string name( n->name());
	
	while( 1)
	{
		std::map<std::string, node_t*>::iterator it( map_.find( name));
	
		if( it == map_.end())
		{
			n->set_name( name);
			map_[name] = n;
			return;
		}
		
		increment_string( name);
	}
}

void unique_name_map_t::remove( node_t *n) { map_.erase( n->name());}

node_t *unique_name_map_t::find( const std::string& name)
{
	std::map<std::string, node_t*>::iterator it( map_.find( name));
	
	if( it == map_.end())
		return 0;
	
	return it->second;
}

std::string unique_name_map_t::get_number( const std::string& str) const
{
    try
    {
	// this is a quick fix, if all str are nums
	int num = boost::lexical_cast<int>( str);
	return str;
    }
    catch( ...) {}

    std::string::size_type n = str.length() - 1;

    while( n != 0)
    {
	char c = str[n];

	if( !isdigit( c))
		break;

	--n;
    }

    return std::string( str, n + 1, str.length());
}

void unique_name_map_t::increment_string( std::string& str) const
{
    std::string num_str = get_number( str);

    if( num_str.empty())
        str.append( "_2");
    else
    {
        std::string base( str, 0, str.length() - num_str.length());
        int num = boost::lexical_cast<int>( num_str);
        num_str = boost::lexical_cast<std::string>( ++num);
        str = base + num_str;
    }
}

} // namespace
