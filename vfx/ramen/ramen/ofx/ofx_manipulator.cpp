// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/ofx_manipulator.hpp>

#include<cassert>

#include<QMouseEvent>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/ui/viewer.hpp>
#include<ramen/ui/palette.hpp>

namespace ramen
{
namespace ofx
{

ofx_manipulator_t::ofx_manipulator_t( ofx_node_t *node, image_effect_t *effect) : manipulator_t(), OFX::Host::Interact::Instance( effect->getOverlayDescriptor( 8, true),
                                                                                                                   (void*) effect)
{
    assert( node && "null ofx_node in ofx_manipulator_t");
    assert( effect && "null image effect in ofx_manipulator_t");

    is_pen_down_ = false;
    is_pen_moving_ = false;
    OfxStatus stat = createInstanceAction();

    if( stat != kOfxStatOK && stat != kOfxStatReplyDefault)
	throw std::runtime_error( "Error creating OFX overlay interact");
}

ofx_manipulator_t::~ofx_manipulator_t() {}

const ofx_node_t *ofx_manipulator_t::ofx_node() const
{
    const ofx_node_t *node = dynamic_cast<const ofx_node_t*>( parent());
    assert( node);
    return node;
}

ofx_node_t *ofx_manipulator_t::ofx_node()
{
    ofx_node_t *node = dynamic_cast<ofx_node_t*>( parent());
    assert( node);
    return node;
}

void ofx_manipulator_t::draw_overlay( ui::image_view_t& view) const
{
    view.save_projection();
    view.set_ofx_projection();

    OfxPointD render_scale;
    render_scale.x = 1;
    render_scale.y = 1;

    ofx_manipulator_t *this_ = const_cast<ofx_manipulator_t*>( this);
    this_->drawAction( ofx_node()->composition()->frame(), render_scale);

    view.restore_projection();
}

bool ofx_manipulator_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)
{ 
    return false;
}

bool ofx_manipulator_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)
{ 
    return false;
}

void ofx_manipulator_t::mouse_enter_event( ui::image_view_t& view, QEvent *event)
{
    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();
    gainFocusAction( ofx_node()->composition()->frame(), render_scale);
}

void ofx_manipulator_t::mouse_leave_event( ui::image_view_t& view, QEvent *event)
{
    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();
    loseFocusAction( ofx_node()->composition()->frame(), render_scale);
}

bool ofx_manipulator_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    parent()->param_set().begin_edit();

    is_pen_down_ = true;
    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();

    OfxPointI mouse_pos;
    mouse_pos.x = view.screen_current_mouse_pos.x;
    mouse_pos.y = view.screen_current_mouse_pos.y;

    OfxPointD pen_pos;
    Imath::V2f world( ofx_node()->vertical_flip_point( view.world_current_mouse_pos));
    world /= view.subsample();
    pen_pos.x = world.x;
    pen_pos.y = world.y;

    penDownAction( ofx_node()->composition()->frame(), render_scale, pen_pos, mouse_pos, 1);
    return true;
}

void ofx_manipulator_t::mouse_move_event( ui::image_view_t& view, QMouseEvent *event)
{
    is_pen_moving_ = true;
    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();

    OfxPointI mouse_pos;
    mouse_pos.x = view.screen_current_mouse_pos.x;
    mouse_pos.y = view.screen_current_mouse_pos.y;

    OfxPointD pen_pos;
    Imath::V2f world( ofx_node()->vertical_flip_point( view.world_current_mouse_pos));
    world /= view.subsample();
    pen_pos.x = world.x;
    pen_pos.y = world.y;

    penMotionAction( ofx_node()->composition()->frame(), render_scale, pen_pos, mouse_pos, 0);
    is_pen_moving_ = false;
}

void ofx_manipulator_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();

    OfxPointI mouse_pos;
    mouse_pos.x = view.screen_current_mouse_pos.x;
    mouse_pos.y = view.screen_current_mouse_pos.y;

    OfxPointD pen_pos;
    Imath::V2f world( ofx_node()->vertical_flip_point( view.world_current_mouse_pos));
    world /= view.subsample();
    pen_pos.x = world.x;
    pen_pos.y = world.y;

    penMotionAction( ofx_node()->composition()->frame(), render_scale, pen_pos, mouse_pos, 1);
}

void ofx_manipulator_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    ofx_node_t *node = dynamic_cast<ofx_node_t*>( parent());
    assert( node);

    OfxPointD render_scale;
    render_scale.x = 1.0 / view.subsample();
    render_scale.y = 1.0 / view.subsample();

    OfxPointI mouse_pos;
    mouse_pos.x = view.screen_current_mouse_pos.x;
    mouse_pos.y = view.screen_current_mouse_pos.y;

    OfxPointD pen_pos;
    Imath::V2f world( ofx_node()->vertical_flip_point( view.world_current_mouse_pos));
    world /= view.subsample();
    pen_pos.x = world.x;
    pen_pos.y = world.y;

    penUpAction(ofx_node()->composition()->frame(), render_scale, pen_pos, mouse_pos, 0);
    is_pen_down_ = false;

    bool is_dirty = !node->param_set().is_command_empty();
    node->param_set().end_edit( false);

    if( is_dirty)
    {
        node->notify();
        node->notify_dependents();
    }
}

void ofx_manipulator_t::getViewportSize(double &width, double &height) const
{
    width  = ui::viewer_t::Instance().image_view()->width();
    height = ui::viewer_t::Instance().image_view()->height();
}

void ofx_manipulator_t::getPixelScale(double& xScale, double& yScale) const
{
    xScale = yScale = 1.0 / ui::viewer_t::Instance().image_view()->pixel_scale();
}

void ofx_manipulator_t::getBackgroundColour(double &r, double &g, double &b) const
{
    Imath::Color3c col( ui::palette_t::Instance()["background"]);
    r = col.x / 255.0;
    r = col.y / 255.0;
    r = col.z / 255.0;
}

OfxStatus ofx_manipulator_t::swapBuffers()
{
    ui::viewer_t::Instance().image_view()->swapBuffers();
    return kOfxStatOK;
}

OfxStatus ofx_manipulator_t::redraw()
{
    ui::viewer_t::Instance().image_view()->update();
    return kOfxStatOK;
}

} // namespace
} // namespace
