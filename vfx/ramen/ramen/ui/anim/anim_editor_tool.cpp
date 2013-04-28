// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor_tool.hpp>

#include<QKeyEvent>
#include<QMouseEvent>

#include<ramen/ui/anim/anim_curves_view.hpp>

namespace ramen
{
namespace ui
{

anim_editor_tool_t::anim_editor_tool_t() {}

void anim_editor_tool_t::draw_overlay( anim_curves_view_t& view) const {}
void anim_editor_tool_t::key_press_event( anim_curves_view_t& view, QKeyEvent *event)	    { event->ignore();}
void anim_editor_tool_t::key_release_event( anim_curves_view_t& view, QKeyEvent *event)	    { event->ignore();}
void anim_editor_tool_t::mouse_press_event( anim_curves_view_t& view, QMouseEvent *event)   { event->ignore();}
void anim_editor_tool_t::mouse_move_event( anim_curves_view_t& view, QMouseEvent *event)    { event->ignore();}
void anim_editor_tool_t::mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event)    { event->ignore();}
void anim_editor_tool_t::mouse_release_event( anim_curves_view_t& view, QMouseEvent *event) { event->ignore();}

} // namespace
} // namespace
