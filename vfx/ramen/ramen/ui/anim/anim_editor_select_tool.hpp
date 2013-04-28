// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_SELECT_TOOL_HPP
#define	RAMEN_UI_ANIM_EDITOR_SELECT_TOOL_HPP

#include<ramen/ui/anim/anim_editor_tool.hpp>

#include<memory>

#include<ramen/ui/anim/anim_editor_command.hpp>

#include<ramen/anim/curve.hpp>

namespace ramen
{
namespace ui
{

class anim_editor_select_tool_t : public anim_editor_tool_t
{
public:

    anim_editor_select_tool_t();

    virtual void draw_overlay( anim_curves_view_t& view) const;

    virtual void key_press_event( anim_curves_view_t& view, QKeyEvent *event);
    virtual void key_release_event( anim_curves_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( anim_curves_view_t& view, QMouseEvent *event);

private:

    void drag_left_tangent( anim_curves_view_t& view, QMouseEvent *event);
    void drag_right_tangent( anim_curves_view_t& view, QMouseEvent *event);

    int push_x_, push_y_;
    int last_x_, last_y_;

    bool dragged_;

    anim::curve_t *curve_;
    anim::keyframe_t *key_;

    // tangent
    bool drag_tangents_mode_;
    bool left_;
    anim::curve_t::iterator key_iter_;

    // points
    bool drag_keyframes_mode_;

    bool insert_keyframe_;

    // box
    bool box_pick_mode_;

    std::auto_ptr<undo::anim_editor_command_t> command_;
};

} // namespace
} // namespace

#endif	/* _ANIM_EDITOR_SELECT_TOOL_HPP */
