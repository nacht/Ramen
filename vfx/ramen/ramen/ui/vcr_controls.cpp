// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/vcr_controls.hpp>

#include<QHBoxLayout>
#include<QToolButton>
#include<QPushButton>
#include<QApplication>
#include<QMouseEvent>
#include<QKeyEvent>

#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector.hpp>
#include<ramen/ui/viewer.hpp>

#include<ramen/flipbook/flipbook.hpp>
#include<ramen/render/render_flipbook.hpp>


namespace ramen
{
namespace ui
{

vcr_controls_impl::vcr_controls_impl() : window_(0), stop_playing_( true)
{
    window_ = new QWidget();
    window_->resize( width(), height());
    window_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    window_->setMinimumSize( width(), height());
    window_->setMaximumSize( width(), height());
    window_->setWindowTitle( QObject::tr( "vcr_controls"));
    window_->setContentsMargins( 0, 0, 0, 0);

    QHBoxLayout *layout = new QHBoxLayout( window_);
    layout->setSpacing( 0);

    start_ = new QToolButton();
    start_->setIcon( QIcon( ":icons/rewind.png"));
    start_->setFocusPolicy( Qt::NoFocus);
    connect( start_, SIGNAL( pressed()), this, SLOT( goto_start()));
    layout->addWidget( start_);

    prev_key_ = new QToolButton();
    prev_key_->setIcon( QIcon( ":icons/prev_key.png"));
    prev_key_->setFocusPolicy( Qt::NoFocus);
    prev_key_->setEnabled( false);
    connect( prev_key_, SIGNAL( pressed()), this, SLOT( prev_key()));
    layout->addWidget( prev_key_);

    prev_frame_ = new QToolButton();
    prev_frame_->setIcon( QIcon( ":icons/prev_frame.png"));
    prev_frame_->setFocusPolicy( Qt::NoFocus);
    connect( prev_frame_, SIGNAL( pressed()), this, SLOT( prev_frame()));
    layout->addWidget( prev_frame_);

    play_back_ = new QToolButton();
    play_back_->setIcon( QIcon( ":icons/play_back.png"));
    play_back_->setFocusPolicy( Qt::NoFocus);
    play_back_->setCheckable( true);
    connect( play_back_, SIGNAL( pressed()), this, SLOT( play_back()));
    layout->addWidget( play_back_);

    stop_ = new QToolButton();
    stop_->setIcon( QIcon( ":icons/stop.png"));
    stop_->setFocusPolicy( Qt::NoFocus);
    layout->addWidget( stop_);

    play_fwd_ = new QToolButton();
    play_fwd_->setIcon( QIcon( ":icons/play.png"));
    play_fwd_->setFocusPolicy( Qt::NoFocus);
    play_fwd_->setCheckable( true);
    connect( play_fwd_, SIGNAL( pressed()), this, SLOT( play_fwd()));
    layout->addWidget( play_fwd_);

    next_frame_ = new QToolButton();
    next_frame_->setIcon( QIcon( ":icons/next_frame.png"));
    next_frame_->setFocusPolicy( Qt::NoFocus);
    connect( next_frame_, SIGNAL( pressed()), this, SLOT( next_frame()));
    layout->addWidget( next_frame_);

    next_key_ = new QToolButton();
    next_key_->setIcon( QIcon( ":icons/next_key.png"));
    next_key_->setFocusPolicy( Qt::NoFocus);
    next_key_->setEnabled( false);
    connect( next_key_, SIGNAL( pressed()), this, SLOT( next_key()));
    layout->addWidget( next_key_);

    end_ = new QToolButton();
    end_->setIcon( QIcon( ":icons/fastfwd.png"));
    end_->setFocusPolicy( Qt::NoFocus);
    connect( end_, SIGNAL( pressed()), this, SLOT( goto_end()));
    layout->addWidget( end_);

    QSpacerItem *horizontal_spacer = new QSpacerItem(40, height(), QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( horizontal_spacer);

    flipbook_ = new QPushButton();
    flipbook_->setText( "Flipbook");
    flipbook_->setEnabled( false);
    connect( flipbook_, SIGNAL( pressed()), this, SLOT( make_flipbook()));
    layout->addWidget( flipbook_);
}

int vcr_controls_impl::width() const
{ 
    return inspector_t::Instance().widget()->width();
}

int vcr_controls_impl::height() const { return 64;}

bool vcr_controls_impl::eventFilter( QObject *watched, QEvent *event)
{
    if( event->type() == QEvent::KeyPress)
    {
	QKeyEvent *key_event = dynamic_cast<QKeyEvent*>( event);

	if( key_event->key() == Qt::Key_Escape)
	{
	    stop_playing();
	    return true;
	}
    }

    if( event->type() == QEvent::MouseButtonPress)
    {
	stop_playing();
	return true;
    }

    return QObject::eventFilter( watched, event);
}

void vcr_controls_impl::update()
{
    flipbook_->setEnabled( document_t::Instance().selected_node() != 0);
}

void vcr_controls_impl::goto_start()
{
    user_interface_t::Instance().set_frame( user_interface_t::Instance().start_frame());
}

void vcr_controls_impl::prev_frame()
{
    int frame = user_interface_t::Instance().frame() - 1;

    if( frame < user_interface_t::Instance().start_frame())
	frame = user_interface_t::Instance().start_frame();

    user_interface_t::Instance().set_frame( frame);
}

void vcr_controls_impl::prev_key() {}

void vcr_controls_impl::play_back()
{
    stop_playing_ = false;

    play_fwd_->blockSignals( true);
    play_fwd_->setChecked( false);
    play_fwd_->blockSignals( false);

    document_t::Instance().composition().set_playing( true);

    qApp->installEventFilter( this);

    while( !stop_playing_)
    {
	int frame = user_interface_t::Instance().frame() - 1;

	if( frame < user_interface_t::Instance().start_frame())
	    frame = user_interface_t::Instance().end_frame();

	user_interface_t::Instance().set_frame( frame);
	qApp->processEvents();
    }
}

void vcr_controls_impl::play_fwd()
{
    stop_playing_ = false;

    play_back_->blockSignals( true);
    play_back_->setChecked( false);
    play_back_->blockSignals( false);

    document_t::Instance().composition().set_playing( true);

    qApp->installEventFilter( this);

    while( !stop_playing_)
    {
	int frame = user_interface_t::Instance().frame() + 1;

	if( frame > user_interface_t::Instance().end_frame())
	    frame = user_interface_t::Instance().start_frame();

	user_interface_t::Instance().set_frame( frame);
	qApp->processEvents();
    }
}

void vcr_controls_impl::next_key() {}

void vcr_controls_impl::next_frame()
{
    int frame = user_interface_t::Instance().frame() + 1;

    if( frame > user_interface_t::Instance().end_frame())
	frame = user_interface_t::Instance().end_frame();

    user_interface_t::Instance().set_frame( frame);
}

void vcr_controls_impl::goto_end()
{
    user_interface_t::Instance().set_frame( user_interface_t::Instance().end_frame());
}

void vcr_controls_impl::make_flipbook()
{
    node_t *n = document_t::Instance().selected_node();
    flipbook::flipbook_t *f = new flipbook::flipbook_t();
    f->setWindowTitle( "Flipbook");
    f->set_frame_rate( document_t::Instance().composition().frame_rate());

    int start       = document_t::Instance().composition().start_frame();
    int end         = document_t::Instance().composition().end_frame();
    int subsample   = document_t::Instance().composition().subsample();

    if( render::render_flipbook( f, n, start, end, viewer_t::Instance().mblur_active(), subsample))
        f->play();
}

void vcr_controls_impl::stop_playing()
{
    play_fwd_->blockSignals( true);
    play_fwd_->setChecked( false);
    play_fwd_->blockSignals( false);

    play_back_->blockSignals( true);
    play_back_->setChecked( false);
    play_back_->blockSignals( false);

    qApp->removeEventFilter( this);
    stop_playing_ = true;
    document_t::Instance().composition().set_playing( false);
}

} // namespace
} // namespace
