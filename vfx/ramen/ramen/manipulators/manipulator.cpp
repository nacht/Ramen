// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/manipulator.hpp>

#include<cassert>

namespace ramen
{

manipulator_t::manipulator_t() { set_parent( 0);}
manipulator_t::~manipulator_t() {}

void manipulator_t::draw_overlay( ui::image_view_t& view) const {}

bool manipulator_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)       { return false;}
bool manipulator_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)     { return false;}

void manipulator_t::mouse_enter_event( ui::image_view_t& view, QEvent *event)        {}
void manipulator_t::mouse_leave_event( ui::image_view_t& view, QEvent *event)        {}
bool manipulator_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)   { return false;}
void manipulator_t::mouse_move_event( ui::image_view_t& view, QMouseEvent *event)    {}
void manipulator_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)    {}
void manipulator_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event) {}

} // namespace
