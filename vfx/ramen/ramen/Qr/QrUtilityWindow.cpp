// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrUtilityWindow.hpp>

#include<QCloseEvent>
#include<QHideEvent>
#include<QShowEvent>

QrUtilityWindow::QrUtilityWindow() : QWidget() {}

void QrUtilityWindow::closeEvent( QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void QrUtilityWindow::toggle()
{
    if( isVisible())
	hide();
    else
	show();
}
