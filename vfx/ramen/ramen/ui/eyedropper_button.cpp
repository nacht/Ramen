// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/eyedropper_button.hpp>

#include<ramen/ui/image_view.hpp>

#include<QApplication>
#include<QMouseEvent>
#include<QKeyEvent>

#include<iostream>

namespace ramen
{
namespace ui
{

eyedropper_button_t::eyedropper_button_t( QWidget *parent) : QPushButton( QIcon( ":icons/eyedropper.png"), "", parent)
{
    setFocusPolicy( Qt::NoFocus);
    setCheckable( true);
    setChecked( false);
    connect( this, SIGNAL( toggled( bool)), this, SLOT( tool_selected( bool)));
}

bool eyedropper_button_t::eventFilter( QObject *watched, QEvent *event)
{
    if( event->type() == QEvent::KeyPress)
    {
	QKeyEvent *key_event = dynamic_cast<QKeyEvent*>( event);

	if( key_event->key() == Qt::Key_Escape)
	{
	    qApp->removeEventFilter( this);
	    setChecked( false);
	    QApplication::restoreOverrideCursor();
	}
    }

    if( event->type() == QEvent::MouseButtonPress)
    {
	QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>( event);
	ui::image_view_t *p = dynamic_cast<ui::image_view_t*>( watched);

	if( p)
	{
	    QPoint local_pos = p->mapFromGlobal( mouse_event->globalPos());
	    image::pixel_t px = p->color_at( local_pos.x(), local_pos.y());
	    value_ = QrColor(	boost::gil::get_color( px, boost::gil::red_t()),
				boost::gil::get_color( px, boost::gil::green_t()),
				boost::gil::get_color( px, boost::gil::blue_t()),
				boost::gil::get_color( px, boost::gil::alpha_t()));
	}
	else
	{
	    // TODO: get color here
	    value_ = QrColor( 0, 0, 0);
	}

	qApp->removeEventFilter( this);
	setChecked( false);
	QApplication::restoreOverrideCursor();
	color_picked( value_);
	return true;
    }

    return QObject::eventFilter( watched, event);
}

void eyedropper_button_t::tool_selected( bool b)
{
    if( b)
    {
	QApplication::setOverrideCursor( QCursor( Qt::CrossCursor));
	qApp->installEventFilter( this);
    }
}

} // namespace
} // namespace
