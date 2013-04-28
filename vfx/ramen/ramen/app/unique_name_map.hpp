// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UNIQUE_NAME_MAP_HPP
#define RAMEN_UNIQUE_NAME_MAP_HPP

#include<map>
#include<string>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

class unique_name_map_t
{
public:

    unique_name_map_t() {}

    void swap( unique_name_map_t& other);

    void insert( node_t *n);
    void remove( node_t *n);

    node_t *find( const std::string& name);

private:

    std::string get_number( const std::string& str) const;
    void increment_string( std::string& str) const;

    std::map<std::string, node_t*> map_;
};

} // namespace

#endif
