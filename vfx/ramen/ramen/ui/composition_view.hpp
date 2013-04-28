// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_COMPOSITION_VIEW_HPP
#define	RAMEN_UI_COMPOSITION_VIEW_HPP

#include<boost/function.hpp>

#include<ramen/ui/viewport.hpp>

#include<QWidget>
#include<QFont>

#include<ramen/ui/draw_composition.hpp>

namespace ramen
{

class node_t;

namespace ui
{

class composition_view_t : public QWidget
{
    Q_OBJECT

public:

    composition_view_t( QWidget *parent = 0);
    ~composition_view_t();

    Imath::V2f screen_to_world( const Imath::V2i& p) const;
    Imath::V2i world_to_screen( const Imath::V2f& p) const;

    void place_node( node_t *n) const;
    void place_node_near_node( node_t *n, node_t *other) const;

protected:

    virtual bool event( QEvent *event);
    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);
    virtual void mouseDoubleClickEvent( QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);
    virtual void paintEvent ( QPaintEvent *event);
    virtual void resizeEvent( QResizeEvent *event);

private:

    void scroll_drag_handler( QMouseEvent *event);
    void zoom_drag_handler( QMouseEvent *event);

    void move_nodes_drag_handler( QMouseEvent *event);

    void connect_drag_handler( QMouseEvent *event);
    void connect_release_handler( QMouseEvent *event);

    void box_pick_drag_handler( QMouseEvent *event);
    void box_pick_release_handler( QMouseEvent *event);

    bool first_resize_;
    ui::viewport_t viewport_;
    QFont font_;

    int push_x_, push_y_;
    int last_x_, last_y_;

    bool scroll_mode_;
    bool zoom_mode_;
    Imath::V2f zoom_center_;

    bool connect_mode_;
    bool box_pick_mode_;

    std::pair<composition_t::reverse_node_iterator, int> last_pick_;
    boost::function<void ( QMouseEvent *)> drag_handler_, release_handler_;
};

} // namespace
} // namespace

#endif

