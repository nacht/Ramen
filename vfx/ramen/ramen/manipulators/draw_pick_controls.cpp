// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/draw_pick_controls.hpp>

#include<cmath>

#include<ramen/GL/gl.hpp>

#include<ramen/ui/image_view.hpp>

namespace ramen
{

void gl_transformed_box( const Imath::Box2i& box, const Imath::M33d& m)
{
    Imath::V2d p = Imath::V2d( box.min.x, box.min.y) * m;
    glVertex2f( p.x, p.y);

    p = Imath::V2d( box.max.x, box.min.y) * m;
    glVertex2f( p.x, p.y);

    p = Imath::V2d( box.max.x, box.max.y) * m;
    glVertex2f( p.x, p.y);

    p = Imath::V2d( box.min.x, box.max.y) * m;
    glVertex2f( p.x, p.y);
}

//void gl_circle( const Imath::V2f& c, float radius, int step)
//{
//    const float deg2rad = 3.14159f / 180.0f;
//    float ang = 0;
//    float ang_inc =
//
//    glBegin( )
//    for (int i = 0; i < 360; i++)
//      //glVertex2f( c.x + cos(degInRad)*radius,sin(degInRad)*radius);
//
//   glEnd();
//}

void draw_point_control( const Imath::V2f& p, float size)
{
    glBegin( GL_LINES);
        glVertex2f( p.x - size, p.y);
        glVertex2f( p.x + size, p.y);

        glVertex2f( p.x, p.y - size);
        glVertex2f( p.x, p.y + size);
    glEnd();
}

void draw_arrow( const Imath::V2f& p, const Imath::V2f& q)
{
    const float deg2rad = 0.01745329251f;
    float l = ( q - p).length() / 7.0f;

    glBegin( GL_LINES);
        glVertex2f( p.x, p.y);
        glVertex2f( q.x, q.y);

        Imath::V2f d( p - q);
        d.normalize();
        d *= l;

        float cs = std::cos( 30.0 * deg2rad);
        float ss = std::sin( 30.0 * deg2rad);

        glVertex2f( q.x, q.y);
        glVertex2f( q.x + ( d.x * cs - d.y * ss), q.y + ( d.x * ss + d.y * cs));

        cs = std::cos( -30.0 * deg2rad);
        ss = std::sin( -30.0 * deg2rad);

        glVertex2f( q.x, q.y);
        glVertex2f( q.x + ( d.x * cs - d.y * ss), q.y + ( d.x * ss + d.y * cs));
    glEnd();
}

void draw_xy_axes( const Imath::V2f& c, float size, float angle, const Imath::Color3f& xcol, const Imath::Color3f& ycol)
{
    const float deg2rad = 0.01745329251f;
    float cs = std::cos( (double) angle * deg2rad);
    float ss = std::sin( (double) angle * deg2rad);

    glColor3f( xcol.x, xcol.y, xcol.z);
    Imath::V2f q( size * cs, size * ss);
    draw_arrow( c, c + q);

    glColor3f( ycol.x, ycol.y, ycol.z);
    q = Imath::V2f( size * ss, -size * cs);
    draw_arrow( c, c + q);
}

} // namespace
