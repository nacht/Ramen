// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/ColorPicker/QrExposurePicker.hpp>

#include<QPaintEvent>
#include<QMouseEvent>
#include<QResizeEvent>
#include<QPainter>

QrExposurePicker::QrExposurePicker( QWidget *parent) : QWidget( parent), background_( sizeHint(), QImage::Format_RGB32)
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy( Qt::StrongFocus);
    valid_background_ = false;
    exposure_ = 0;
}

QSize QrExposurePicker::sizeHint() const { return QSize( 200, 15);}

double QrExposurePicker::max_exposure() { return 20.0;}

double QrExposurePicker::exposure() const { return exposure_;}

void QrExposurePicker::setExposure( double e)
{
    if( exposure_ != e)
    {
	exposure_ = e;
	exposureChanged( exposure_);
	update();
    }
}

void QrExposurePicker::paintEvent( QPaintEvent *event)
{
    QPainter painter( this);

    if( event->rect().intersects(contentsRect()))
        painter.setClipRegion( event->region().intersect(contentsRect()));
    
    if( !valid_background_)
    {
	updateBackground();
	valid_background_ = true;
    }

    painter.drawImage( contentsRect().topLeft(), background_);

    int xpos = ( exposure_ / ( 2.0 * max_exposure()) + 0.5) * ( width() - 1);

    QPen pen;
    pen.setWidth( 1);

    pen.setColor( QColor( 255, 255, 255));
    painter.setPen( pen);
    painter.drawLine( xpos, 0, xpos, height());

    ++xpos;
    pen.setColor( QColor( 0, 0, 0));
    painter.setPen( pen);
    painter.drawLine( xpos, 0, xpos, height());
}

void QrExposurePicker::mousePressEvent( QMouseEvent *event)
{
    setExposure( pickExposure( event->x()));
    event->accept();
}

void QrExposurePicker::mouseMoveEvent( QMouseEvent *event)
{
    setExposure( pickExposure( event->x()));
    event->accept();
}

void QrExposurePicker::mouseReleaseEvent( QMouseEvent *event) { event->accept();}

void QrExposurePicker::resizeEvent( QResizeEvent *event)
{
    valid_background_ = false;
    update();
}

double QrExposurePicker::pickExposure( int x) const
{
    double t = (double) x / ( width() - 1);
    return (t - 0.5) * 2.0 * max_exposure();
}

void QrExposurePicker::updateBackground()
{
    background_ = QImage(contentsRect().size(), QImage::Format_RGB32);

    QColor col;

    for( int j = 0; j < background_.height(); ++j)
    {
	QRgb *p = reinterpret_cast<QRgb*>( background_.scanLine( j));

	double l = 0;
	double l_inc = 1.0 / ( double) ( width() - 1);

	for( int i = 0; i < background_.width(); ++i)
	{
	    *p++ = qRgb( l * 255.0, l * 255.0, l * 255.0);
	    l += l_inc;
	}
    }
}
