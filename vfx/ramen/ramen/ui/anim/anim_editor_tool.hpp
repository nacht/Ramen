// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_TOOL_HPP
#define	RAMEN_UI_ANIM_EDITOR_TOOL_HPP

class QKeyEvent;
class QMouseEvent;

namespace ramen
{
namespace ui
{

class anim_curves_view_t;

class anim_editor_tool_t
{
public:

    anim_editor_tool_t();

    virtual void draw_overlay( anim_curves_view_t& view) const;

    virtual void key_press_event( anim_curves_view_t& view, QKeyEvent *event);
    virtual void key_release_event( anim_curves_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( anim_curves_view_t& view, QMouseEvent *event);
};

} // namespace
} // namespace

#endif	/* _ANIM_EDITOR_TOOLS_HPP */
