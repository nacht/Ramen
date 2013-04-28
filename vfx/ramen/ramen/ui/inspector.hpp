// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_INSPECTOR_HPP
#define	RAMEN_UI_INSPECTOR_HPP

#include<map>

#include<boost/noncopyable.hpp>
#include<boost/signal.hpp>

#include<loki/Singleton.h>

#include<QScrollArea>

#include<ramen/Qr/QrUtilityWindow.hpp>
#include<ramen/Qr/QrContainerWidget.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

namespace ui
{

class panel_t;

class inspector_impl : boost::noncopyable
{
public:

    // metrics
    int left_margin() const;
    int width() const;

    void show();
    void hide();
    void toggle();

    void edit_node( node_t *n);
    void delete_panel( node_t *n);

    void update();

    QWidget *widget() { return window_;}

private:

    inspector_impl();
    ~inspector_impl();

    void create_panel( node_t *n);

    void recreate_panel( node_t *n);

    friend struct Loki::CreateUsingNew<inspector_impl>;

    // metrics
    mutable int left_margin_;
    mutable int width_;

    QrUtilityWindow *window_;
    QScrollArea *scroll_;
    QrContainerWidget *view_;

    std::map<node_t*,panel_t*> panels_;
    std::map<node_t*,panel_t*>::iterator current_;

    boost::signals::connection connection_;
};

typedef Loki::SingletonHolder<inspector_impl> inspector_t;

} // namespace
} // namespace

#endif	/* _INSPECTOR_HPP */
