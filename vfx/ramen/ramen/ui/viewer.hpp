// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_VIEWER_HPP
#define	RAMEN_UI_VIEWER_HPP

#include<map>

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include<QObject>

#include<ramen/Qr/QrUtilityWindow.hpp>
#include<ramen/Qr/QrContainerWidget.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>

#include<ramen/ui/image_view.hpp>
#include<ramen/nodes/node.hpp>

class QWidget;
class QFrame;
class QPushButton;
class QComboBox;
class QDoubleSpinBox;
class QToolButton;
class QrSmallToolButton;
class QrLabel;

namespace ramen
{
namespace ui
{

enum view_node_t
{
    view_active_node = 0,
    view_context_node
};

class viewer_impl : public QObject
{
    Q_OBJECT

public:

    QWidget *widget() { return window_;}

    void show();
    void hide();
    void toggle();

    int toolbar_height() const { return 30;}

    view_node_t view_mode() const { return view_mode_;}
    void set_view_mode( view_node_t v) { view_mode_ = v;}

    bool mblur_active() const;

    void set_active_node( node_t *n);
    void set_context_node( node_t *n);

    void node_released( node_t *n);

    void frame_changed();

    image_view_t *image_view() { return view_;}

    void set_resolution( int index);

    void set_status( const std::string& text);

public Q_SLOTS:

     void autoupdate_toggle( bool state);

     void change_view( int index);
     void change_resolution( int index);
     void change_channels( int index);

     void mblur_toggle( bool state);

     void checks_toggle( bool state);
     void roi_toggle( bool state);
     void overlay_toggle( bool state);
     void normal_range_toggle( bool state);

     void change_exposure( double d);

private:

    viewer_impl();
    ~viewer_impl();

    void active_node_changed();
    void context_node_changed();

    friend struct Loki::CreateUsingNew<viewer_impl>;

    QrUtilityWindow *window_;

    QrLabel *status_;

    image_view_t *view_;

    QrContainerWidget *active_toolbar_;

    QWidget *viewer_controls_;
    QToolButton *update_btn_;
    QComboBox *result_combo_;
    QComboBox *resolution_combo_;
    QComboBox *channels_combo_;
    QrSmallToolButton *mblur_btn_;
    QrSmallToolButton *show_checks_btn_;
    QrSmallToolButton *show_interest_btn_;
    QrSmallToolButton *show_overlay_btn_;
    QrSmallToolButton *normal_range_btn_;
    QrDoubleSpinBox *exposure_input_;

    view_node_t view_mode_;

    bool pending_update_;
    bool autoupdate_;

    node_t::connection_type active_connection_;
    node_t::connection_type context_connection_;

    std::map<node_t*,QWidget*> toolbars_;
    std::map<node_t*,QWidget*>::iterator current_;
};

typedef Loki::SingletonHolder<viewer_impl> viewer_t;

} // ui
} // ramen

#endif	/* _VIEWER_HPP */
