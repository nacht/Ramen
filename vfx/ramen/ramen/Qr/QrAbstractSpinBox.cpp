// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrAbstractSpinBox.hpp>

#include<QLineEdit>
#include<QMouseEvent>
#include<QResizeEvent>

#include<QHBoxLayout>

#include<ramen/Qr/QrCalculator.hpp>

#include<iostream>

QrAbstractSpinBox::QrAbstractSpinBox( QWidget *parent) : QWidget( parent)
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);
    edit_ = new QLineEdit();
    edit_->setFocusPolicy( Qt::NoFocus);
    edit_->setReadOnly( true);
    edit_->installEventFilter( this);
    edit_->setMouseTracking( false);
    layout->addWidget( edit_);
    setLayout( layout);
    mouse_tracking_ = true;
}

bool QrAbstractSpinBox::eventFilter( QObject *watched, QEvent *event)
{
    if( !isEnabled())
	return QObject::eventFilter( watched, event);

    switch( event->type())
    {
    case QEvent::MouseButtonPress:
	mousePressEvent( (QMouseEvent *) event);
	return true;

    case QEvent::MouseMove:
	mouseDragEvent( (QMouseEvent *) event);
	return true;

    case QEvent::MouseButtonRelease:
	mouseReleaseEvent( (QMouseEvent *) event);
	return true;

    default:
	return QObject::eventFilter( watched, event);
    }
}

void QrAbstractSpinBox::mousePressEvent( QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();

    gpush_x_ = event->globalX();
    gpush_y_ = event->globalY();

    dragged_ = false;
    first_time_drag_ = true;
    event->accept();
}

void QrAbstractSpinBox::mouseDragEvent( QMouseEvent *event)
{
    if( ( event->x() == last_x_) && ( event->y() == last_y_))
    {
	// ignore
	event->accept();
	return;
    }
    
    if( first_time_drag_)
    {
	// add a bit of filtering here, for wacom tablets
	if( abs( event->x() - push_x_) < 3)
	{
	    last_x_ = event->x();
	    last_y_ = event->y();
	    event->accept();
	    return;
	}

	dragged_ = true;
	first_time_drag_ = false;

	if( mouseTracking())
	    spinBoxPressed();
    }
    else
	mouseDragged( event->x() - last_x_, mouseTracking());

    last_x_ = event->x();
    last_y_ = event->y();
    event->accept();
}

void QrAbstractSpinBox::mouseReleaseEvent( QMouseEvent *event)
{
    if( !dragged_)
    {
	QrCalculator::Instance().setValue( doubleValue());
	connect( &(QrCalculator::Instance()), SIGNAL( valueChanged( double)), this, SLOT( calculatorSetValue( double)));
	QrCalculator::Instance().show( gpush_x_, gpush_y_);
    }
    else
    {
	if( mouseTracking())
	    spinBoxReleased();
	else
	    emitValueChangedSignal();
    }

    event->accept();
}

void QrAbstractSpinBox::setLineEditValue( int v)
{
    QString s;
    s.setNum( v);
    edit_->setText( s);
}

void QrAbstractSpinBox::setLineEditValue( double v, int prec)
{
    QString s;
    s.setNum( v, 'g', prec);
    edit_->setText( s);
}

QSize QrAbstractSpinBox::sizeHint() const
{
    QSize s = edit_->sizeHint();
    QFontMetrics fm( font());
    s.setWidth( s.width() - fm.width( QLatin1Char('x')) * 8);
    return s;
}

QSize QrAbstractSpinBox::minimumSizeHint() const { return edit_->minimumSizeHint();}
