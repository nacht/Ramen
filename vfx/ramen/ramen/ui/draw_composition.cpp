// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/draw_composition.hpp>

#include<algorithm>

#include<boost/range.hpp>

#include<QPainter>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/composition_view.hpp>
#include<ramen/ui/palette.hpp>

namespace ramen
{
namespace ui
{

const int node_min_width = 90;
const int node_extra_width = 20;
const int node_height = 20;
const int shadow_offset = 3;
const int radius = 4;

int node_width( const node_t& n)
{
    return node_min_width + node_extra_width * std::max( (int) n.num_inputs() - 3, 0);
}

Imath::V2f input_location( const node_t& n, std::size_t i)
{
    float off = node_width( n) / ( n.num_inputs() + 1);
    return Imath::V2f( n.location().x + (i+1)*off, n.location().y - 1);
}

Imath::V2f output_location( const node_t& n)
{
    return Imath::V2f( n.location().x + node_width( n) / 2, n.location().y + node_height + 2);
}

void draw_node( QPainter& painter, const node_t& n)
{
    Imath::V2f p = n.location();

    QPen pen;
    QBrush brush;

    brush.setStyle( Qt::SolidPattern);
    brush.setColor( QColor( 64, 64, 64, 255));

    painter.setPen( Qt::NoPen);
    painter.setBrush( brush);
    painter.drawRoundedRect( QRectF( p.x + shadow_offset, p.y + shadow_offset, node_width( n), node_height), radius, radius);

    for( unsigned int i=0;i<n.num_inputs();++i)
    {
	brush.setColor( QColor( n.plugs_info()[i].color().x, n.plugs_info()[i].color().y, n.plugs_info()[i].color().z));
	painter.setBrush( brush);
	Imath::V2f q = input_location( n, i);
	painter.drawRect( q.x - 3, q.y - 3, 6, 6);
    }

    if( n.has_output_plug())
    {
	Imath::Color3c col( palette_t::Instance()["out plug"]);
	brush.setColor( QColor( col.x, col.y, col.z));
	painter.setBrush( brush);
	Imath::V2f q = output_location( n);
	painter.drawRect( q.x - 3, q.y - 3, 6, 6);
    }

    if( n.selected())
	brush.setColor( QColor( 200, 140, 0, 255));
    else
    {
        if( n.plugin_error())
            brush.setColor( QColor( 150, 25, 0, 255));
        else
            brush.setColor( QColor( 150, 150, 150, 255));
    }

    painter.setBrush( brush);
    painter.drawRoundedRect( QRectF( p.x, p.y, node_width( n), node_height), radius, radius);

    painter.setBrush( Qt::NoBrush);

    if( user_interface_t::Instance().active_node() == &n)
	pen.setColor( QColor( 255, 0, 0));
    else
    {
        if( user_interface_t::Instance().context_node() == &n)
	    pen.setColor( QColor( 0, 0, 255));
	else
	    pen.setColor( QColor( 255, 255, 255));
    }

    painter.setPen( pen);
    painter.drawRoundedRect( QRectF( p.x, p.y, node_width( n), node_height), radius, radius);

    pen.setColor( QColor( 255, 255, 255));
    painter.setPen( pen);
    painter.drawText( QRectF( p.x, p.y + 3, node_width( n), node_height - 3), Qt::AlignCenter, QString( n.name().c_str()));

    if( n.ignored())
    {
        pen.setColor( QColor( 255, 0, 0));
	pen.setWidth(2);
	painter.setPen( pen);
	painter.drawLine( QPointF( p.x - 3, p.y - 2), QPointF( p.x + node_width( n) + 3, p.y + node_height + 2));
    }
}

void draw_edge( QPainter& painter, const composition_t::edge_type& e)
{
    Imath::V2f s( output_location( *(e.src)));
    Imath::V2f d( input_location( *(e.dst), e.port));
    painter.drawLine( QPointF( s.x, s.y), QPointF( d.x, d.y));
}

// picking
int pick( const composition_view_t& view, const node_t& n, const Imath::V2f& p)
{
    if( ( p.x < n.location().x) || (p.x > n.location().x + node_width( n)))
	    return no_pick;

    if( ( p.y < n.location().y - 6.0) || ( p.y > n.location().y + node_height + 5))
	    return no_pick;

    Imath::V2f q( output_location( n));

    Imath::V2i pi( view.world_to_screen( p));
    Imath::V2i qi( view.world_to_screen( q));

    if( ( pi - qi).length2() < 25)
	    return output_picked;

    for( unsigned int i=0;i<n.num_inputs();++i)
    {
	q = input_location( n, i);
	qi = view.world_to_screen( q);

	if( ( pi - qi).length2() < 25)
		return i;
    }

    return body_picked;
}

bool pick( const composition_view_t& view, const node_t& n, const Imath::Box2f& b)
{
    if( n.location().x + node_width( n) < b.min.x)
	return false;

    if( n.location().x > b.max.x)
	return false;

    if( n.location().y > b.max.y)
	return false;

    if( n.location().y + node_height < b.min.y)
	return false;

    return true;
}

bool pick( const composition_view_t& view, const composition_t::edge_type& e, const Imath::V2f& p)
{
    Imath::V2f s( output_location( *(e.src)));
    Imath::V2f d( input_location( *(e.dst), e.port));

    float u = ((p.x - s.x) * (d.x - s.x) + (p.y - s.y) * (d.y - s.y)) / ( s - d).length2();

    float nx = s.x + u * (d.x - s.x);
    float ny = s.y + u * (d.y - s.y);

    if( (u < 0) || (u > 1))
	return false;

    if( ((nx - p.x) * (nx - p.x)) + ((ny - p.y) * (ny - p.y)) < 9)
	return true;

    return false;
}

std::pair<composition_t::reverse_node_iterator, int> pick( const composition_view_t& view, composition_t::node_range_type& r, const Imath::V2f& p)
{
    for( composition_t::reverse_node_iterator it( r.rbegin()); it != r.rend(); ++it)
    {
	int result = pick( view, *it, p);

	if( result != no_pick)
	    return std::make_pair( it, result);
    }

    return std::pair<composition_t::reverse_node_iterator, int>( r.rend(), no_pick);
}

std::pair<composition_t::reverse_edge_iterator, bool> pick( const composition_view_t& view, composition_t::edge_range_type& r, const Imath::V2f& p)
{
    for( composition_t::reverse_edge_iterator it( r.rbegin()); it != r.rend(); ++it)
    {
	if( pick( view, *it, p))
	    return std::make_pair( it, true);
    }
	
    return std::pair<composition_t::reverse_edge_iterator, int>( r.rend(), false);
}

} // namespace
} // namespace
