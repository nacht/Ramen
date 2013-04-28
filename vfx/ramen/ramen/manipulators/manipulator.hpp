// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULATOR_HPP
#define RAMEN_MANIPULATOR_HPP

#include<boost/noncopyable.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/manipulators/manipulable_fwd.hpp>

#include<ramen/ui/image_view_fwd.hpp>

class QEvent;
class QKeyEvent;
class QMouseEvent;

namespace ramen
{

/**
 * @class manipulator_t
 * @brief base class for on-screen controls shown in the viewer.
 *
 * base class for on-screen controls shown in the viewer.
 */

class manipulator_t : boost::noncopyable
{
public:

    manipulator_t();
    virtual ~manipulator_t();

    const manipulable_t *parent() const { return parent_;}
    manipulable_t *parent()             { return parent_;}
    void set_parent( manipulable_t *p)  { parent_ = p;}

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_enter_event( ui::image_view_t& view, QEvent *event);
    virtual void mouse_leave_event( ui::image_view_t& view, QEvent *event);

    virtual bool mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

private:

    manipulable_t *parent_;
};

} // namespace

#endif
