// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_MANIPULATOR_HPP
#define RAMEN_OFX_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

#include"ofxCore.h"
#include"ofxInteract.h"

#include"ofxhPropertySuite.h"
#include"ofxhInteract.h"

namespace ramen
{

class ofx_node_t;

namespace ofx
{

class image_effect_t;

class ofx_manipulator_t : public manipulator_t, public OFX::Host::Interact::Instance
{
public:

    ofx_manipulator_t( ofx_node_t *node, image_effect_t *effect);
    ~ofx_manipulator_t();

    const ofx_node_t *ofx_node() const;
    ofx_node_t *ofx_node();

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_enter_event( ui::image_view_t& view, QEvent *event);
    virtual void mouse_leave_event( ui::image_view_t& view, QEvent *event);

    virtual bool mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

    bool is_pen_down() const	{ return is_pen_down_;}
    bool is_pen_moving() const	{ return is_pen_moving_;}

    // Interact implementation
    virtual void getViewportSize(double &width, double &height) const;
    virtual void getPixelScale(double& xScale, double& yScale) const;
    virtual void getBackgroundColour(double &r, double &g, double &b) const;

    virtual OfxStatus swapBuffers();
    virtual OfxStatus redraw();

private:

    //float vertical_flip( float y) const;

    bool is_pen_down_;
    bool is_pen_moving_;
};

} // namespace
} // namespace

#endif
