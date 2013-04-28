// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_PANEL_HPP
#define	RAMEN_UI_PANEL_HPP

#include<QObject>

class QWidget;
class QLineEdit;

namespace ramen
{

class node_t;

namespace ui
{

class panel_t : public QObject
{
    Q_OBJECT

public:

    panel_t( node_t *n);
    ~panel_t();

    QWidget *widget() { return panel_;}

    void update();

private Q_SLOTS:

    void change_name();
    void set_autokey( bool b);
    void reset();
    void show_help();

private:

    node_t *n_;
    QWidget *panel_;
    QLineEdit *edit_;
};

} // namespace
} // namespace

#endif
