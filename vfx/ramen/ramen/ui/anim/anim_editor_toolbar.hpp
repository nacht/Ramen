// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_TOOLBAR_HPP
#define	RAMEN_UI_ANIM_EDITOR_TOOLBAR_HPP

#include<QWidget>

#include<ramen/ui/anim/anim_editor_tool.hpp>

namespace ramen
{
namespace ui
{

class anim_editor_toolbar_t : public QWidget
{
    Q_OBJECT

public:

    anim_editor_toolbar_t( QWidget *parent = 0);
    ~anim_editor_toolbar_t();

    anim_editor_tool_t *tool();

public Q_SLOTS:

    void set_tangents_smooth();
    void set_tangents_flat();
    void set_tangents_linear();
    void set_tangents_step();

    void set_extrapolation_constant();
    void set_extrapolation_linear();
    void set_extrapolation_repeat();

    void frame_selection();
    void frame_all();

private:

    anim_editor_tool_t *select_tool_;
};

} // namespace
} // namespace

#endif	/* _ANIM_EDITOR_TOOLBAR_HPP */
