// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrColorButton.hpp>

QrColorButton::QrColorButton( QWidget *parent) : QPushButton( parent), value_( 0, 0, 0, 0)
{
    updateButtonColor();
}

void QrColorButton::setValue( const QrColor& c)
{
    value_ = c;
    updateButtonColor();
    valueChanged( value_);
}

void QrColorButton::setRed( double x)
{
    value_.setRed( x);
    updateButtonColor();
    valueChanged( value_);
}

void QrColorButton::setGreen( double x)
{
    value_.setGreen( x);
    updateButtonColor();
    valueChanged( value_);
}

void QrColorButton::setBlue( double x)
{
    value_.setBlue( x);
    updateButtonColor();
    valueChanged( value_);
}

void QrColorButton::updateButtonColor()
{
    QString str = "* { background-color: rgb( %1, %2, %3) }";
    QColor col = value_;
    setStyleSheet( str.arg( col.red()).arg( col.green()).arg( col.blue()));
}
