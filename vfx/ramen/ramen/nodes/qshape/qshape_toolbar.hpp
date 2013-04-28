// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_TOOLBAR_HPP
#define	RAMEN_QSHAPE_TOOLBAR_HPP

#include<memory>

#include<QWidget>

#include<ramen/undo/command.hpp>

#include<ramen/nodes/qshape/qshape_node.hpp>

class QCheckBox;
class QPushButton;
class QButtonGroup;
class QrDoubleSpinBox;
class QToolButton;
class QrSmallToolButton;
class QrColorButton;

namespace ramen
{

namespace qshape
{

class shape_t;

class qshape_toolbar_t : public QWidget
{
    Q_OBJECT
    
public:

    qshape_toolbar_t( qshape_node_t *parent);
    ~qshape_toolbar_t();

    void update_widgets();

    void param_changed( param_t *p);

public Q_SLOTS:

    void pick_select_tool( bool b);
    void pick_create_tool( bool b);
    void pick_convert_tool( bool b);

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_moved( double value);
    void spinbox_released();

    void move_shape_layer_up();
    void move_shape_layer_down();

    void toggle_active( bool b);

    void change_shape_color();

private:

    void selection_changed( shape_t *s);

    qshape_node_t *parent_;

    QButtonGroup *bgroup_;

    QCheckBox *active_;

    QrSmallToolButton *select_, *create_, *convert_;
    QToolButton *autokey_, *order_up_, *order_down_;

    QrColorButton *color_button_;

    QrDoubleSpinBox *color_, *opacity_, *grow_, *blur_;

    node_t::connection_type connection_;

    std::auto_ptr<undo::command_t> command_;

    bool first_drag_;
};

} // namespace
} // namespace

#endif	/* _qshape_TOOLBAR_HPP */
