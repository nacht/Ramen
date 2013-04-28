// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULABLE_HPP
#define RAMEN_MANIPULABLE_HPP

#include<ramen/params/parameterised.hpp>

#include<ramen/ui/image_view_fwd.hpp>

class QKeyEvent;
class QMouseEvent;

namespace ramen
{

class manipulable_t : public parameterised_t
{
public:

    manipulable_t();
    manipulable_t( const manipulable_t& other);

    virtual ~manipulable_t();

    void draw( ui::image_view_t& view) const            { do_draw( view);}
    void draw_overlay( ui::image_view_t& view) const    { do_draw_overlay( view);}

    bool key_press_event( ui::image_view_t& view, QKeyEvent *event)	{ return do_key_press_event( view, event);}
    bool key_release_event( ui::image_view_t& view, QKeyEvent *event)   { return do_key_release_event( view, event);}

    bool mouse_enter_event( ui::image_view_t& view, QEvent *event)  { return do_mouse_enter_event( view, event);}
    bool mouse_leave_event( ui::image_view_t& view, QEvent *event)  { return do_mouse_leave_event( view, event);}

    bool mouse_press_event( ui::image_view_t& view, QMouseEvent *event)     { return do_mouse_press_event( view, event);}
    bool mouse_move_event( ui::image_view_t& view, QMouseEvent *event)      { return do_mouse_move_event( view, event);}
    bool mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)      { return do_mouse_drag_event( view, event);}
    bool mouse_release_event( ui::image_view_t& view, QMouseEvent *event)   { return do_mouse_release_event( view, event);}

private:

    virtual void do_draw( ui::image_view_t& view) const	    {}
    virtual void do_draw_overlay( ui::image_view_t& view) const {}

    virtual bool do_key_press_event( ui::image_view_t& view, QKeyEvent *event)	{ return false;}
    virtual bool do_key_release_event( ui::image_view_t& view, QKeyEvent *event){ return false;}

    virtual bool do_mouse_enter_event( ui::image_view_t& view, QEvent *event)	{ return false;}
    virtual bool do_mouse_leave_event( ui::image_view_t& view, QEvent *event)	{ return false;}

    virtual bool do_mouse_press_event( ui::image_view_t& view, QMouseEvent *event)  { return false;}
    virtual bool do_mouse_move_event( ui::image_view_t& view, QMouseEvent *event)   { return false;}
    virtual bool do_mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)   { return false;}
    virtual bool do_mouse_release_event( ui::image_view_t& view, QMouseEvent *event){ return false;}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
