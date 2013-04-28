// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/ColorPicker/QrColorSwatch.hpp>

#include<cmath>

#include<QPainter>
#include<QPaintEvent>
#include<QResizeEvent>

QrColorSwatch::QrColorSwatch( QWidget *parent, const QrColor& c) : QWidget( parent), background_( sizeHint(), QImage::Format_RGB32)
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy( Qt::StrongFocus);
    valid_background_ = false;
    color_ = c;
}

QSize QrColorSwatch::sizeHint() const { return QSize( 100, 100);}

void QrColorSwatch::setColor( QrColor c)
{
    color_ = c;
    valid_background_ = false;
    update();
}

void QrColorSwatch::paintEvent( QPaintEvent *event)
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
}

void QrColorSwatch::resizeEvent( QResizeEvent *event)
{
    valid_background_ = false;
    update();
}

void QrColorSwatch::updateBackground()
{
    const double soft_start = 0.5;
    const double soft_end = 0.85;
    const double center = width()/2;

    background_ = QImage( contentsRect().size(), QImage::Format_RGB32);

    QrColor nl_col = color_;
    nl_col.applyGamma( 1.0 / 2.2);
    nl_col.clamp();
    nl_col *= 255.0;

    for( int j = 0; j < background_.height(); ++j)
    {
	QRgb *p = reinterpret_cast<QRgb*>( background_.scanLine( j));

	for( int i = 0; i < background_.width(); ++i)
	{
	    double r = std::sqrt( (double) (( i - center) * ( i - center)) + (( j - center) * ( j - center))) / center;

	    if( r > soft_end)
		*p = qRgb( 0, 0, 0);
	    else
	    {
		if( r < soft_start)
		    *p = qRgb(	nl_col.red(), nl_col.green(), nl_col.blue());
		else
		{
		    double s = 1.0 - ( ( r - soft_start) / ( soft_end - soft_start));

		    QrColor col = color_;
		    col *= s;
		    col.applyGamma( 1.0 / 2.2);
		    col.clamp();
		    col *= 255;
		    *p = qRgb( col.red(), col.green(), col.blue());
		}
	    }

	    ++p;
	}
    }
}
