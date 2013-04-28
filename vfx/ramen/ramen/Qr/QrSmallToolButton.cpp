// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrSmallToolButton.hpp>

#ifdef __APPLE__
#include<QMacStyle>
#endif

QrSmallToolButton::QrSmallToolButton( QWidget* parent) : QToolButton( parent)
{
    #ifdef __APPLE__
    QMacStyle::setWidgetSizePolicy( this, QMacStyle::SizeSmall);
    setMinimumSize(QSize(20, 20));
    setMaximumSize(QSize(20, 20));
    #else
    setMinimumSize(QSize(25, 25));
    setMaximumSize(QSize(25, 25));
    #endif
}
