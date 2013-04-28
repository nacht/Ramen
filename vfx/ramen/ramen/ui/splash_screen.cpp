// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/splash_screen.hpp>

#include<QApplication>
#include<QSplashScreen>

namespace ramen
{
namespace ui
{

splash_screen_impl::splash_screen_impl() : splash_(0)
{
    QPixmap pixmap( ":/splash.jpg");
    splash_ = new QSplashScreen( pixmap);
    splash_->show();
}

splash_screen_impl::~splash_screen_impl()
{
    if( splash_)
	splash_->deleteLater();
}

void splash_screen_impl::show()
{
    splash_->show();
    qApp->processEvents();
}

void splash_screen_impl::show_message( const std::string& msg)
{
    splash_->showMessage( QString::fromStdString( msg));
    qApp->processEvents();
}

void splash_screen_impl::finish( QWidget *w) { splash_->finish( w);}

} // namespace
} // namespace
