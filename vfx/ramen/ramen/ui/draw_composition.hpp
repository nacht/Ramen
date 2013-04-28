// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DRAW_COMPOSITION_HPP
#define RAMEN_DRAW_COMPOSITION_HPP

#include<utility>

#include<OpenEXR/ImathBox.h>

#include<ramen/app/composition.hpp>

class QPainter;

namespace ramen
{
namespace ui
{

class composition_view_t;

enum
{
    no_pick		= -3,
    output_picked	= -2,
    body_picked		= -1
    // input number n picked
};

void draw_node( QPainter& painter, const node_t& n);
void draw_edge( QPainter& painter, const composition_t::edge_type& e);

// picking

bool pick( const composition_view_t& view, const node_t& n, const Imath::Box2f& b);

std::pair<composition_t::reverse_node_iterator, int>  pick( const composition_view_t& view, composition_t::node_range_type& r, const Imath::V2f& p);
std::pair<composition_t::reverse_edge_iterator, bool> pick( const composition_view_t& view, composition_t::edge_range_type& r, const Imath::V2f& p);

}
}

#endif
