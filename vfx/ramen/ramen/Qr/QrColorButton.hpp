// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QRCOLORBUTTON_HPP
#define	RAMEN_QRCOLORBUTTON_HPP

#include<QPushButton>

#include<ramen/Qr/QrColor.hpp>

class QrColorButton : public QPushButton
{
    Q_OBJECT

public:

    QrColorButton( QWidget *parent = 0);

    const QrColor& value() const { return value_;}

public Q_SLOTS:

    void setValue( const QrColor& c);

    void setRed( double x);
    void setGreen( double x);
    void setBlue( double x);

Q_SIGNALS:

    void valueChanged( const QrColor&);

private:

    void updateButtonColor();

    QrColor value_;
};

#endif
