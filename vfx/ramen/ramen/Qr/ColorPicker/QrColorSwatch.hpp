// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_COLOR_SWATCH_HPP
#define	QR_COLOR_SWATCH_HPP

#include<QWidget>
#include<QImage>

#include<ramen/Qr/QrColor.hpp>

class QrColorSwatch : public QWidget
{
    Q_OBJECT

public:

    QrColorSwatch( QWidget *parent, const QrColor& c);

    QSize sizeHint() const;
    int heightForWidth( int w) const { return w;}

public Q_SLOTS:

    void setColor( QrColor c);

protected:

    void paintEvent( QPaintEvent *event);
    void resizeEvent( QResizeEvent *event);

private:

    void updateBackground();

    inline double clamp( double x) const
    {
	if( x < 0) return 0;
	if( x > 1) return 1;
	return x;
    }

    QImage background_;
    bool valid_background_;

    QrColor color_;
};

#endif