// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_node_view.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/GL/gl.hpp>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/nodes/qshape/shape.hpp>

#include<ramen/ui/image_view.hpp>

namespace ramen
{
namespace qshape
{

node_view_t::node_view_t( qshape_node_t* parent) : parent_( parent) {}

void node_view_t::draw_overlay( ui::image_view_t& view) const
{
    adobe::for_each( parent_->shapes(), boost::bind( &node_view_t::draw_shape, this, _1));
}

void node_view_t::draw_shape( const shape_t& s) const
{
    glColor3f( s.display_color().x, s.display_color().y, s.display_color().z);

    if( s.size() >= 2)
    {
	for( shape_t::const_iterator it( s.begin()); it != s.end()-1; ++it)
	    draw_bezier_curve( it->p1(), it->p2(), (it+1)->p0(), (it+1)->p1());

	if( s.closed())
	    draw_bezier_curve( s.back().p1(), s.back().p2(), s.front().p0(), s.front().p1());
    }

    if( s.selected())
	draw_control_polygon( s);
}

void node_view_t::draw_bezier_curve( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c, const Imath::V2f& d) const
{
GLfloat ctrlpoints[4][3];

    ctrlpoints[0][0] = a.x;
    ctrlpoints[0][1] = a.y;
    ctrlpoints[0][2] = 0;

    ctrlpoints[1][0] = b.x;
    ctrlpoints[1][1] = b.y;
    ctrlpoints[1][2] = 0;

    ctrlpoints[2][0] = c.x;
    ctrlpoints[2][1] = c.y;
    ctrlpoints[2][2] = 0;

    ctrlpoints[3][0] = d.x;
    ctrlpoints[3][1] = d.y;
    ctrlpoints[3][2] = 0;

    glMap1f( GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);

    glBegin(GL_LINE_STRIP);
    for (int i=0;i<=30;i++)
	glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
}

void node_view_t::draw_control_polygon( const shape_t& s) const
{
    adobe::for_each( s.triples(), boost::bind( &node_view_t::draw_triple, this, _1, s.display_color()));
}

void node_view_t::draw_triple( const triple_t& t, const Imath::Color3f& col) const
{
    glPointSize( 5);

    if( t.selected())
	glColor3f( 1, 0, 0);
    else
	glColor3f( col.x, col.y, col.z);

    glBegin( GL_POINTS);
//    if( t.corner())
//	glVertex2f( t.p1().x, t.p1().y);
//    else
//    {
        glVertex2f( t.p0().x, t.p0().y);
	glVertex2f( t.p1().x, t.p1().y);
	glVertex2f( t.p2().x, t.p2().y);
//    }
    glEnd();

//    if( !t.corner())
//    {
        glBegin( GL_LINES);
        glVertex2f( t.p0().x, t.p0().y);
        glVertex2f( t.p1().x, t.p1().y);
        glVertex2f( t.p1().x, t.p1().y);
        glVertex2f( t.p2().x, t.p2().y);
	glEnd();
    //}
}

} // namespace
} // namespace
