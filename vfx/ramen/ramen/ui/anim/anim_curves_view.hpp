// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVES_VIEW_HPP
#define	RAMEN_ANIM_CURVES_VIEW_HPP

#include<QWidget>

#include<vector>

#include<ramen/ui/viewport.hpp>

#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/curve_fwd.hpp>

class QPainter;

namespace ramen
{
namespace ui
{

class anim_curves_view_t : public QWidget
{
    Q_OBJECT

public:

    anim_curves_view_t( QWidget *parent = 0);
    ~anim_curves_view_t();

    float time_scale() const;
    float value_scale() const;

    Imath::V2i world_to_screen( const Imath::V2f& p) const;
    Imath::V2f screen_to_world( const Imath::V2i& p) const;

    QPainter *painter() const { return painter_;}

    const ui::viewport_t& viewport() const	{ return viewport_;}
    ui::viewport_t& viewport()		{ return viewport_;}

    void frame_area( const Imath::Box2f& area);

protected:

    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);
    virtual void mouseDoubleClickEvent( QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);
    virtual void paintEvent ( QPaintEvent *event);
    virtual void resizeEvent( QResizeEvent *event);

private:

    void draw_grid() const;
    void draw_axes() const;
    void draw_time_bar() const;

    // from paul heckberts' code
    double nice_num( double x, bool round) const;

    int abs( int x) const
    {
	if( x < 0)
	    return -x;
	
	return x;
    }

    bool first_resize_;
    ui::viewport_t viewport_;

    int push_x_, push_y_;
    int last_x_, last_y_;

    bool scroll_mode_;
    bool zoom_mode_;
    Imath::V2f zoom_center_;

    bool move_time_mode_;

    QPainter *painter_;
};

} // namespace
} // namespace

#endif	/* _ANIM_CURVES_VIEW_HPP */
