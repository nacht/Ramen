// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_VCR_CONTROLS_HPP
#define	RAMEN_UI_VCR_CONTROLS_HPP

#include<map>

#include<boost/noncopyable.hpp>
#include<boost/signal.hpp>

#include<loki/Singleton.h>

#include<QWidget>

class QToolButton;
class QPushButton;

namespace ramen
{
namespace ui
{

class panel_t;

class vcr_controls_impl : public QObject
{
    Q_OBJECT

public:

    QWidget *widget() { return window_;}

    int width() const;
    int height() const;

    bool eventFilter( QObject *watched, QEvent *event);

    void update();

public Q_SLOTS:

    void goto_start();
    void prev_frame();
    void prev_key();
    void play_back();

    void play_fwd();
    void next_key();
    void next_frame();
    void goto_end();

    void make_flipbook();

private:

    vcr_controls_impl();

    void stop_playing();

    friend struct Loki::CreateUsingNew<vcr_controls_impl>;

    QWidget *window_;

    QToolButton *start_, *prev_key_, *prev_frame_, *play_back_;
    QToolButton *stop_, *play_fwd_, *next_frame_, *next_key_, *end_;

    QPushButton *flipbook_;
    
    bool stop_playing_;
};

typedef Loki::SingletonHolder<vcr_controls_impl> vcr_controls_t;

} // namespace
} // namespace

#endif	/* _vcr_controls_HPP */
