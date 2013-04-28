// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#ifndef RAMEN_UI_PY_CONSOLE_HPP
#define	RAMEN_UI_PY_CONSOLE_HPP

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include<QObject>

class QWidget;
class QTextEdit;

namespace ramen
{
namespace ui
{

class python_console_impl : public QObject, boost::noncopyable
{
    Q_OBJECT

public:

    QWidget *widget() { return window_;}

    QTextEdit *edit_area()	{ return edit_;}
    QTextEdit *output_area()	{ return output_;}

    void write( const std::string& str);

private Q_SLOTS:

    void run();

private:

    python_console_impl();
    ~python_console_impl();

    friend struct Loki::CreateUsingNew<python_console_impl>;

    QWidget *window_;
    QTextEdit *edit_;
    QTextEdit *output_;
};

typedef Loki::SingletonHolder<python_console_impl> python_console_t;

} // namespace
} // namespace

#endif	/* _lua_console_HPP */
