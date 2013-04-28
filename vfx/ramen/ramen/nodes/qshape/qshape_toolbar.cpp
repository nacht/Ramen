// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_toolbar.hpp>

#include<iostream>

#include<boost/bind.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QHBoxLayout>
#include<QCheckBox>
#include<QPushButton>
#include<QSpacerItem>
#include<QButtonGroup>
#include<QToolButton>
#include<QColorDialog>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>
#include<ramen/Qr/QrSmallToolButton.hpp>
#include<ramen/Qr/QrColorButton.hpp>

#include<ramen/nodes/qshape/qshape_commands.hpp>
#include<ramen/nodes/qshape/shape.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/viewer.hpp>

#include<ramen/undo/undo_stack.hpp>

namespace ramen
{
namespace qshape
{

qshape_toolbar_t::qshape_toolbar_t( qshape_node_t *parent) : parent_( parent), QWidget()
{
    setMinimumSize( 0, ui::viewer_t::Instance().toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, ui::viewer_t::Instance().toolbar_height());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);

    select_ = new QrSmallToolButton( this);
    select_->setIcon( QIcon( ":icons/arrow.png"));
    select_->setFocusPolicy( Qt::NoFocus);
    select_->setCheckable(true);
    select_->setChecked(true);
    select_->setToolTip( tr( "Select / drag curves and points"));
    connect( select_, SIGNAL( toggled( bool)), this, SLOT( pick_select_tool( bool)));
    layout->addWidget( select_);

    create_ = new QrSmallToolButton( this);
    create_->setIcon( QIcon( ":icons/pen.png"));
    create_->setFocusPolicy( Qt::NoFocus);
    create_->setCheckable(true);
    create_->setChecked(false);
    create_->setToolTip( tr( "Create curves"));
    connect( create_, SIGNAL( toggled( bool)), this, SLOT( pick_create_tool( bool)));
    layout->addWidget( create_);

    convert_ = new QrSmallToolButton( this);
    convert_->setIcon( QIcon( ":icons/linear_tan.png"));
    convert_->setFocusPolicy( Qt::NoFocus);
    convert_->setCheckable(true);
    convert_->setChecked(false);
    convert_->setToolTip( tr( "Convert points corner / curve"));
    connect( convert_, SIGNAL( toggled( bool)), this, SLOT( pick_convert_tool( bool)));
    layout->addWidget( convert_);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    active_ = new QCheckBox( "Active");
    active_->setToolTip( tr( "Render shape on / off"));
    connect( active_, SIGNAL( toggled( bool)), this, SLOT( toggle_active( bool)));
    layout->addWidget( active_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    color_button_ = new QrColorButton();
    color_button_->setFocusPolicy( Qt::NoFocus);
    QSize sz = color_button_->sizeHint();
    sz.setWidth( sz.height());
    color_button_->setMinimumSize( sz);
    color_button_->setMaximumSize( sz);
    color_button_->setValue( QrColor( 1, 1, 0));
    color_button_->setToolTip( tr( "Display color"));
    connect( color_button_, SIGNAL( pressed()), this, SLOT( change_shape_color()));
    layout->addWidget( color_button_);

    QrLabel *label = new QrLabel( "Color");
    layout->addWidget( label);

    color_ = new QrDoubleSpinBox();
    color_->setRange( 0, 1);
    color_->setStep( 0.05);
    sz = color_->sizeHint();
    sz.setWidth( sz.width() * 0.75);
    color_->setMinimumSize( sz);
    color_->setMaximumSize( sz);
    color_->setToolTip( tr( "Shape color"));
    connect( color_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( color_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( color_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( color_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    layout->addWidget( color_);

    label = new QrLabel( "Opacity");
    layout->addWidget( label);

    opacity_ = new QrDoubleSpinBox();
    opacity_->setRange( 0, 1);
    opacity_->setStep( 0.05);
    opacity_->setMinimumSize( sz);
    opacity_->setMaximumSize( sz);
    opacity_->setToolTip( tr( "Shape opacity"));
    connect( opacity_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( opacity_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( opacity_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( opacity_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    layout->addWidget( opacity_);

    label = new QrLabel( "Grow");
    layout->addWidget( label);

    grow_ = new QrDoubleSpinBox();
    grow_->setMinimumSize( sz);
    grow_->setMaximumSize( sz);
    grow_->setToolTip( tr( "Grow/Shrink Shape"));
    connect( grow_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( grow_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( grow_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( grow_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    layout->addWidget( grow_);

    label = new QrLabel( "Blur");
    layout->addWidget( label);

    blur_ = new QrDoubleSpinBox();
    blur_->setMinimum( 0);
    blur_->setMinimumSize( sz);
    blur_->setMaximumSize( sz);
    blur_->setToolTip( tr( "Blur Shape"));
    connect( blur_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( blur_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( blur_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( blur_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    layout->addWidget( blur_);

    order_up_ = new QToolButton();
    order_up_->setFocusPolicy( Qt::NoFocus);
    order_up_->setArrowType( Qt::UpArrow);
    order_up_->setToolTip( tr( "Move Shape up"));
    order_up_->setEnabled( false);
    connect( order_up_, SIGNAL( pressed()), this, SLOT( move_shape_layer_up()));
    layout->addWidget( order_up_);

    order_down_ = new QToolButton();
    order_down_->setFocusPolicy( Qt::NoFocus);
    order_down_->setArrowType( Qt::DownArrow);
    order_down_->setToolTip( tr( "Move Shape down"));
    order_down_->setEnabled( false);
    connect( order_down_, SIGNAL( pressed()), this, SLOT( move_shape_layer_down()));
    layout->addWidget( order_down_);

    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);

    setLayout( layout);

    bgroup_ = new QButtonGroup( this);
    bgroup_->addButton( create_);
    bgroup_->addButton( select_);
    bgroup_->addButton( convert_);

    update_widgets();
    connection_ = parent_->selection_changed.connect( boost::bind( &qshape_toolbar_t::selection_changed, this, _1));
}

qshape_toolbar_t::~qshape_toolbar_t() { connection_.disconnect();}

void qshape_toolbar_t::pick_select_tool( bool b)  { parent_->pick_select_tool();}
void qshape_toolbar_t::pick_create_tool( bool b)  { parent_->pick_create_tool();}
void qshape_toolbar_t::pick_convert_tool( bool b) { parent_->pick_convert_tool();}

void qshape_toolbar_t::toggle_active( bool b)
{
    std::auto_ptr<undo::command_t> c( new undo::qshape_toggle_active_command_t( parent_));
    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void qshape_toolbar_t::value_changed( double value)
{
    parent_->selected()->param_set().begin_edit();

    float time = parent_->composition()->frame();

    if( QObject::sender() == color_)
	parent_->selected()->set_color_at_time( color_->value(), time);

    if( QObject::sender() == opacity_)
	parent_->selected()->set_opacity_at_time( opacity_->value(), time);

    if( QObject::sender() == grow_)
	parent_->selected()->set_grow_at_time( grow_->value(), time);

    if( QObject::sender() == blur_)
	parent_->selected()->set_blur_at_time( blur_->value(), time);

    parent_->selected()->param_set().end_edit();
}

void qshape_toolbar_t::spinbox_pressed() { parent_->selected()->param_set().begin_edit();}

void qshape_toolbar_t::spinbox_moved( double value)
{
    float time = parent_->composition()->frame();

    if( QObject::sender() == color_)
	parent_->selected()->set_color_at_time( color_->value(), time);

    if( QObject::sender() == opacity_)
	parent_->selected()->set_opacity_at_time( opacity_->value(), time);

    if( QObject::sender() == grow_)
	parent_->selected()->set_grow_at_time( grow_->value(), time);

    if( QObject::sender() == blur_)
	parent_->selected()->set_blur_at_time( blur_->value(), time);

    parent_->selected()->param_set().notify_parent();
    ui::anim_editor_t::Instance().update();
}

void qshape_toolbar_t::spinbox_released()
{
    parent_->selected()->param_set().end_edit();
}

void qshape_toolbar_t::move_shape_layer_up()
{
    std::auto_ptr<undo::command_t> c( new undo::qshape_order_command_t( parent_, true));
    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void qshape_toolbar_t::move_shape_layer_down()
{
    std::auto_ptr<undo::command_t> c( new undo::qshape_order_command_t( parent_, false));
    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void qshape_toolbar_t::change_shape_color()
{
    Imath::Color3f col = parent_->selected()->display_color();

    QColor initial( adobe::clamp( col.x, 0.0f, 1.0f) * 255,
		    adobe::clamp( col.y, 0.0f, 1.0f) * 255,
		    adobe::clamp( col.z, 0.0f, 1.0f) * 255);

    QColor c = QColorDialog::getColor( initial, ui::user_interface_t::Instance().main_window());
    parent_->selected()->set_display_color( Imath::Color3f( c.red() / 255.0f, c.green() / 255.0f, c.blue() / 255.0f));
    update_widgets();
    ui::user_interface_t::Instance().update();
    ui::viewer_t::Instance().image_view()->update();
}

void qshape_toolbar_t::selection_changed( shape_t *s) { update_widgets();}

void qshape_toolbar_t::update_widgets()
{
    active_->blockSignals( true);
    color_->blockSignals( true);
    opacity_->blockSignals( true);
    grow_->blockSignals( true);
    blur_->blockSignals( true);
    color_button_->blockSignals( true);

    active_->setEnabled( parent_->selected());
    color_->setEnabled( parent_->selected());
    opacity_->setEnabled( parent_->selected());
    grow_->setEnabled( parent_->selected());
    blur_->setEnabled( parent_->selected());
    color_button_->setEnabled( parent_->selected());

    if( parent_->selected())
    {
	active_->setChecked( parent_->selected()->active());

        color_->setValue( get_value<float>( parent_->selected()->param( "color")));
        opacity_->setValue( get_value<float>( parent_->selected()->param( "opacity")));
        grow_->setValue( get_value<float>( parent_->selected()->param( "grow")));
        blur_->setValue( get_value<float>( parent_->selected()->param( "blur")));

	QrColor col( parent_->selected()->display_color().x,
		    parent_->selected()->display_color().y,
		    parent_->selected()->display_color().z);

	color_button_->setValue( col);

	int index = parent_->index_for_shape( parent_->selected());
	order_up_->setEnabled( index != ( parent_->num_shapes() - 1));
	order_down_->setEnabled( index != 0);
    }
    else
    {
	order_up_->setEnabled( false);
	order_down_->setEnabled( false);
    }

    active_->blockSignals( false);
    color_->blockSignals( false);
    opacity_->blockSignals( false);
    grow_->blockSignals( false);
    blur_->blockSignals( false);
    color_button_->blockSignals( false);
}

void qshape_toolbar_t::param_changed( param_t *p) { update_widgets();}

} // namespace
} // namespace
