// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/QRamenApp.hpp>

#include<QFileOpenEvent>

#include<ramen/ui/user_interface.hpp>

QRamenApp::QRamenApp( int& argc, char **argv) : QApplication( argc, argv) {}

bool QRamenApp::event( QEvent *event)
{
    switch( event->type())
    {
    case QEvent::FileOpen:
    {
	QString fname = static_cast<QFileOpenEvent *>( event)->file();
	boost::filesystem::path p( fname.toStdString());
	ramen::ui::user_interface_t::Instance().open_document( p, true);
	return true;
    }

    default:
	return QApplication::event( event);
    }
}
