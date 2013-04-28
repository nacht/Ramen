// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QRCOLOR_HPP
#define	RAMEN_QRCOLOR_HPP

#include<QColor>

class QrColor
{
public:

    QrColor();
    QrColor( double r, double g, double b, double a = 1.0f);

    operator QColor();

    QrColor& operator*=( double s);

    double red() const	    { return red_;}
    double green() const    { return green_;}
    double blue() const	    { return blue_;}
    double alpha() const    { return alpha_;}

    void setRed( double x)   { red_ = x;}
    void setGreen( double x) { green_ = x;}
    void setBlue( double x)  { blue_ = x;}
    void setAlpha( double x) { alpha_ = x;}

    void applyGamma( double g = 2.2);
    void clamp();

    void toHSV( double& h, double& s, double& v) const;
    static QrColor fromHSV( double h, double s, double v);

private:

    double red_, green_, blue_, alpha_;
};

#endif	/* _QRCOLOR_HPP */

