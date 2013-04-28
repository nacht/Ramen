// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrLabel.hpp>

#ifdef __APPLE__
#include<QMacStyle>
#endif

QrLabel::QrLabel( QWidget *parent, Qt::WindowFlags f) : QLabel( parent, f)
{
    #ifdef __APPLE__
    QMacStyle::setWidgetSizePolicy( this, QMacStyle::SizeSmall);
    #endif
}

QrLabel::QrLabel( const QString & text, QWidget *parent, Qt::WindowFlags f) : QLabel( text, parent, f)
{
    #ifdef __APPLE__
    QMacStyle::setWidgetSizePolicy( this, QMacStyle::SizeSmall);
    #endif
}
