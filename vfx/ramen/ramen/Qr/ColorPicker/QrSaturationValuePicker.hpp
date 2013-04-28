// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_SAT_VALUE_PICKER_HPP
#define	QR_SAT_VALUE_PICKER_HPP

#include<QWidget>
#include<QImage>

class QrSaturationValuePicker : public QWidget
{
    Q_OBJECT

public:

    QrSaturationValuePicker( QWidget *parent = 0);

    QSize sizeHint() const;

    double hue() const		{ return hue_;}
    double saturation() const	{ return saturation_;}
    double value() const	{ return value_;}

Q_SIGNALS:

    void saturationValueChanged( double sat, double val);

public Q_SLOTS:

    void setHue( double h);
    void setSaturationValue( double s, double v);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);

private:

    void pickSatVal( QMouseEvent *event);

    void updateBackground();

    QImage background_;
    bool valid_background_;

    double hue_;
    double saturation_;
    double value_;
};

#endif
