// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_SPLASH_SCREEN_HPP
#define RAMEN_UI_SPLASH_SCREEN_HPP

#include<string>

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include<QSplashScreen>

namespace ramen
{
namespace ui
{

class splash_screen_impl : boost::noncopyable
{
public:

    void show();
    void show_message( const std::string& msg);

    void finish( QWidget *w);

private:

    splash_screen_impl();
    ~splash_screen_impl();

    friend struct Loki::CreateUsingNew<splash_screen_impl>;

    QSplashScreen *splash_;
};

typedef Loki::SingletonHolder<splash_screen_impl> splash_screen_t;

} // namespace
} // namespace

#endif
