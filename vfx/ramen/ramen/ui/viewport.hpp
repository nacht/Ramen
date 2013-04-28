// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_VIEWPORT_HPP
#define	RAMEN_VIEWPORT_HPP

#include<OpenEXR/ImathMatrix.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace ui
{

class viewport_t
{
public:

    viewport_t() : y_down_( false) {}

    bool y_down() const         { return y_down_;}
    void set_y_down( bool b)    { y_down_ = b;}

    const Imath::Box2f& world() const   { return world_;}
    const Imath::Box2i& device() const  { return device_;}

    float zoom_x() const    { return ( device_.size().x)  / ( world_.size().x);}
    float zoom_y() const    { return ( device_.size().y)  / ( world_.size().y);}

    Imath::V2f screen_to_world( const Imath::V2i& p) const
    {
        int y = p.y;

        if( y_down_)
            y = device_.min.y + ( device_.max.y - y);

	return Imath::V2f( ((p.x - device_.min.x) / zoom_x()) + world_.min.x,
			    ((y - device_.min.y) / zoom_y()) + world_.min.y);
    }

    Imath::V2i world_to_screen( const Imath::V2f& p) const
    {
        int x = ((p.x - world_.min.x) * zoom_x()) + device_.min.x;
        int y = ((p.y - world_.min.y) * zoom_y()) + device_.min.y;

         if( y_down_)
            y = device_.min.y + ( device_.max.y - y);

        return Imath::V2i( x, y);
    }

    Imath::V2f screen_to_world_dir( const Imath::V2f& v) const
    {
	return Imath::V2f( v.x / zoom_x(), v.y / zoom_y());
    }

    Imath::Box2f screen_to_world( const Imath::Box2i& b) const
    {
        return Imath::Box2f( screen_to_world( Imath::V2i( b.min.x, b.min.y)), screen_to_world( Imath::V2i( b.max.x, b.max.y)));
    }

    Imath::Box2i world_to_screen( const Imath::Box2f& b) const
    {
        return Imath::Box2i( world_to_screen( Imath::V2f( b.min.x, b.min.y)), world_to_screen( Imath::V2f( b.max.x, b.max.y)));
    }

    Imath::M33f world_to_screen_matrix() const;
    Imath::M33f screen_to_world_matrix() const;

    void reset()                { world_ = Imath::Box2f( Imath::V2i( device_.min.x, device_.min.y), Imath::V2i( device_.max.x, device_.max.y));}

    void reset( int w, int h)   { reset( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w-1, h-1)));}

    void reset( const Imath::Box2i& device)
    {
	device_ = device;
	reset();
    }

    void reset( const Imath::Box2i& device, const Imath::Box2f& world)
    {
	world_ = world;
	device_ = device;
    }

    void resize( const Imath::Box2i& device)
    {
	world_.max.x = world_.min.x + ( device.size().x / zoom_x());
	world_.max.y = world_.min.y + ( device.size().y / zoom_y());
	device_ = device;
    }

    void resize( int w, int h) { resize( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w - 1, h - 1)));}

    void scroll( const Imath::V2i& inc)
    {
        if( y_down_)
            world_ = offsetBy( world_, Imath::V2f( inc.x / zoom_x(), -inc.y / zoom_y()));
        else
            world_ = offsetBy( world_, Imath::V2f( inc.x / zoom_x(),  inc.y / zoom_y()));
    }

    void zoom( const Imath::V2f& center, float factor) { zoom( center, factor, factor);}

    void zoom( const Imath::V2f& center, float xfactor, float yfactor)
    {
        world_ = offsetBy( world_, Imath::V2f( -center.x, -center.y));
        world_.min.x *= xfactor;
        world_.min.y *= yfactor;
        world_.max.x *= xfactor;
        world_.max.y *= yfactor;
        world_ = offsetBy( world_, Imath::V2f( center.x, center.y));
    }

    void scroll_to_center_point( const Imath::V2f& center)
    {
        world_ = offsetBy( world_, center - world_.center());
    }

private:

    Imath::Box2i device_;
    Imath::Box2f world_;
    bool y_down_;
};

} // namespace
} // namespace

#endif
