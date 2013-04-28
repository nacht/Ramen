// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_IMAGE_VIEW_HPP
#define	RAMEN_UI_IMAGE_VIEW_HPP

#include<ramen/ui/image_view_fwd.hpp>

#include<ramen/ui/viewport.hpp>

#include<GL/glew.h>

#include<QGLWidget>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/image/typedefs.hpp>

class QHideEvent;
class QShowEvent;

namespace ramen
{
namespace ui
{

enum image_view_mode
{
    image_view_rgb = 0,
    image_view_red,
    image_view_green,
    image_view_blue,
    image_view_alpha
};

class image_view_t : public QGLWidget
{
    Q_OBJECT

public:

    image_view_t( QWidget *parent = 0);
    virtual ~image_view_t();

    void load_texture();
    void load_texture( node_t *n);

    void delete_texture( void);

    float subsample() const;

    void show_checks_texture( bool b);
    void set_view_mode( image_view_mode m);
    void set_exposure( float e);
    void set_gamma( float g);
    void set_image_zoom( float z);
    void show_overlay( bool b);
    void set_normal_range( bool b);

    void set_active_node( node_t *n);

    float pixel_scale() const;

    const ui::viewport_t& viewport() const { return viewport_;}

    Imath::V2f screen_to_world( const Imath::V2i& p) const;
    Imath::V2i world_to_screen( const Imath::V2f& p) const;

    Imath::Box2f screen_to_world( const Imath::Box2i& b) const;
    Imath::Box2i world_to_screen( const Imath::Box2f& b) const;

    Imath::V2f screen_to_world_dir( const Imath::V2f& v) const;

    image::pixel_t color_at( int x, int y) const;
    //image::const_image_view subimage_view( int x, int y, int w, int h) const;

    void save_projection();
    void restore_projection();
    void set_screen_projection();

    // ofx code
    void set_ofx_projection();

    void draw_selection_box( int x0, int y0, int x1, int y1);

protected:

    virtual void initializeGL();
    virtual void resizeGL( int w, int h);
    virtual void paintGL();

    virtual void enterEvent( QEvent *event);
    virtual void leaveEvent( QEvent *event);

    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);

    virtual void mouseDoubleClickEvent( QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);

    virtual void hideEvent( QHideEvent *event);
    virtual void showEvent( QShowEvent *event);

public:

    Imath::V2i screen_click_pos, screen_last_mouse_pos, screen_current_mouse_pos;
    Imath::V2f world_click_pos, world_last_mouse_pos, world_current_mouse_pos;

    bool dragged;
    bool first_drag;

private:

    void test_gl_extensions();

    void draw_checks() const;
    void draw_image();
    
    bool first_time_;
    bool first_image_loaded_;
    ui::viewport_t viewport_;

    bool scroll_mode_;
    bool zoom_mode_;
    Imath::V2f zoom_center_;
    
    float exposure_, gamma_;
    image_view_mode mode_;
    bool normal_range_;

    bool checks_;
    bool overlay_;

    GLuint alpha_program_, alpha_shader_;
    GLuint color_program_, color_shader_;

    node_t *active_;
    bool event_accepted_by_node_;

    GLuint texture_id_;
    image::image_t image_;
    bool valid_texture_;
    Imath::Box2i domain_, defined_;
};

} // namespace
} // namespace

#endif
