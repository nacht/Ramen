// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_INFO_HPP
#define RAMEN_NODE_INFO_HPP

#include<string>

#include<boost/function.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

struct node_info_t
{
    node_info_t() : create( 0), create_gui( 0), init( 0), cleanup( 0), first_time( true), visible( true)
    {
    }
	
    std::string id;
    int major_version;
    int minor_version;
    std::string submenu;
    std::string menu_item;
    bool visible;

    // more info
    boost::function<node_t *()> create;
    boost::function<node_t *()> create_gui;
    boost::function<void ()> init;
    boost::function<void ()> cleanup;

    bool first_time;
};

}

#endif
