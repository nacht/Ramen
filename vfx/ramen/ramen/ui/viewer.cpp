// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewer.hpp>

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QFrame>
#include<QPushButton>
#include<QComboBox>
#include<QDoubleSpinBox>
#include<QToolButton>
#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrSmallToolButton.hpp>

#include<ramen/app/document.hpp>
#include<ramen/render/renderer.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{

viewer_impl::viewer_impl() : QObject()
{
    view_mode_ = view_active_node;

    autoupdate_ = true;
    pending_update_ = false;

    current_ = toolbars_.end();

    window_ = new QrUtilityWindow();
    window_->move( 850, 30);
    window_->resize( 512, 409);
    window_->setWindowTitle( "Viewer");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 5, 5, 5, 5);

    QWidget *status_bar = new QWidget();
    QHBoxLayout *status_layout = new QHBoxLayout();
    status_layout->setContentsMargins( 0, 0, 0, 0);
    status_ = new QrLabel();
    status_->setText( "Status");
    status_layout->addWidget( status_);

    QSize label_size = status_->sizeHint();
    status_bar->setMinimumSize( 0, label_size.height());
    status_bar->setMaximumSize( QWIDGETSIZE_MAX, label_size.height());

    status_bar->setLayout( status_layout);
    layout->addWidget( status_bar);

    view_ = new image_view_t();
    layout->addWidget( view_);

    active_toolbar_ = new QrContainerWidget();
    active_toolbar_->setMinimumSize( 0, toolbar_height());
    active_toolbar_->setMaximumSize( QWIDGETSIZE_MAX, toolbar_height());
    active_toolbar_->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget( active_toolbar_);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    viewer_controls_ = new QWidget();
    viewer_controls_->setMinimumSize( 0, 30);
    viewer_controls_->setMaximumSize( QWIDGETSIZE_MAX, 30);
    viewer_controls_->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *horizontalLayout = new QHBoxLayout( viewer_controls_);
    horizontalLayout->setContentsMargins( 0, 0, 0, 0);

	update_btn_ = new QrSmallToolButton();
	update_btn_->setFocusPolicy( Qt::NoFocus);
	update_btn_->setCheckable(true);
	update_btn_->setChecked(true);
	update_btn_->setText( "U");
	update_btn_->setToolTip( "Auto-update");
	connect( update_btn_, SIGNAL( toggled( bool)), this, SLOT( autoupdate_toggle( bool)));
	horizontalLayout->addWidget(update_btn_);

	result_combo_ = new QComboBox();
	result_combo_->setFocusPolicy( Qt::NoFocus);
        result_combo_->clear();
	result_combo_->insertItems(0, QStringList() << "Active" << "Context");
	result_combo_->setMinimumSize( result_combo_->sizeHint());
	result_combo_->setToolTip( "Show result / context node");
	connect( result_combo_, SIGNAL( activated( int)), this, SLOT( change_view( int)));
	horizontalLayout->addWidget(result_combo_);

	resolution_combo_ = new QComboBox();
	resolution_combo_->setFocusPolicy( Qt::NoFocus);
        resolution_combo_->clear();
	resolution_combo_->insertItems(0, QStringList() << "Full" << "Half" << "Third" << "Quarter");
	resolution_combo_->setMinimumSize( resolution_combo_->sizeHint());
	resolution_combo_->setToolTip( "Preview resolution");
	connect( resolution_combo_, SIGNAL( activated( int)), this, SLOT( change_resolution( int)));
	horizontalLayout->addWidget(resolution_combo_);

        channels_combo_ = new QComboBox();
	channels_combo_->setFocusPolicy( Qt::NoFocus);
        channels_combo_->clear();
	channels_combo_->insertItems(0, QStringList() << "RGB" << "Red" << "Green" << "Blue" << "Alpha");
	channels_combo_->setToolTip( "Display channels");
	connect( channels_combo_, SIGNAL( activated( int)), this, SLOT( change_channels( int)));
	channels_combo_->setMinimumSize( channels_combo_->sizeHint());
	horizontalLayout->addWidget(channels_combo_);

        mblur_btn_ = new QrSmallToolButton();
	mblur_btn_->setFocusPolicy( Qt::NoFocus);
	mblur_btn_->setText( "M");
	mblur_btn_->setCheckable(true);
	mblur_btn_->setToolTip( "Motion blur on/off");
	connect( mblur_btn_, SIGNAL( toggled( bool)), this, SLOT( mblur_toggle( bool)));
	horizontalLayout->addWidget(mblur_btn_);

        show_checks_btn_ = new QrSmallToolButton();
	show_checks_btn_->setFocusPolicy( Qt::NoFocus);
	show_checks_btn_->setText( "C");
	show_checks_btn_->setCheckable(true);
	show_checks_btn_->setToolTip( "Show checks background");
	connect( show_checks_btn_, SIGNAL( toggled( bool)), this, SLOT( checks_toggle( bool)));
	horizontalLayout->addWidget(show_checks_btn_);

	show_interest_btn_ = new QrSmallToolButton();
	show_interest_btn_->setFocusPolicy( Qt::NoFocus);
	show_interest_btn_->setText( "I");
	show_interest_btn_->setCheckable( true);
	show_interest_btn_->setEnabled( false);
	show_interest_btn_->setToolTip( "Show region of interest");
	connect( show_interest_btn_, SIGNAL( toggled( bool)), this, SLOT( roi_toggle( bool)));
        horizontalLayout->addWidget(show_interest_btn_);

	show_overlay_btn_ = new QrSmallToolButton();
	show_overlay_btn_->setFocusPolicy( Qt::NoFocus);
	show_overlay_btn_->setText( "O");
	show_overlay_btn_->setCheckable(true);
	show_overlay_btn_->setChecked(true);
	show_overlay_btn_->setToolTip( "Show overlay");
	connect( show_overlay_btn_, SIGNAL( toggled( bool)), this, SLOT( overlay_toggle( bool)));
	horizontalLayout->addWidget(show_overlay_btn_);

	normal_range_btn_ = new QrSmallToolButton();
	normal_range_btn_->setFocusPolicy( Qt::NoFocus);
	normal_range_btn_->setCheckable(true);
	normal_range_btn_->setChecked(true);
	normal_range_btn_->setText( "N");
	normal_range_btn_->setToolTip( "Display 0-1 color range");
	connect( normal_range_btn_, SIGNAL( toggled( bool)), this, SLOT( normal_range_toggle( bool)));
	horizontalLayout->addWidget(normal_range_btn_);

	QrLabel *label = new QrLabel();
	label->setText( "Exp");
	horizontalLayout->addWidget( label);

	exposure_input_ = new QrDoubleSpinBox();
	exposure_input_->setFocusPolicy( Qt::NoFocus);
	QSize s = exposure_input_->sizeHint();
	exposure_input_->setMinimumSize( s);
	exposure_input_->setMaximumSize( s);
	exposure_input_->setMinimum(-30.0);
	exposure_input_->setMaximum( 30.0);
	exposure_input_->setStep( 0.1);
	exposure_input_->setEnabled( false);
	exposure_input_->setToolTip( "Viewer Exposure");
	connect( exposure_input_, SIGNAL( valueChanged( double)), this, SLOT( change_exposure( double)));
	horizontalLayout->addWidget(exposure_input_);

	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);

    viewer_controls_->setLayout( horizontalLayout);
    layout->addWidget( viewer_controls_);
    window_->setLayout( layout);
}

viewer_impl::~viewer_impl() {}

void viewer_impl::show()
{ 
    if(!window_->isVisible())
    {
	window_->show();

	if(pending_update_)
	{
	    if( view_mode_ == view_active_node)
		active_node_changed();
	    else
		context_node_changed();
	}
    }
}
void viewer_impl::hide()
{
    if( window_->isVisible())
        window_->hide();
}

void viewer_impl::toggle()
{ 
    if( window_->isVisible())
	hide();
    else
	show();
}

bool viewer_impl::mblur_active() const { return mblur_btn_->isChecked();}

void viewer_impl::set_active_node( node_t *n)
{
    active_connection_.disconnect();

    if( current_ != toolbars_.end())
    {
	active_toolbar_->clearContents();
	current_ = toolbars_.end();
    }

    image_view()->set_active_node( n);

    if(n != 0)
    {
	active_connection_ = n->changed.connect( boost::bind(&viewer_impl::active_node_changed, this));

	std::map<node_t*,QWidget*>::const_iterator it = toolbars_.find( n);

	if( it == toolbars_.end())
	{
	    QWidget *toolbar = n->create_toolbar();
	    toolbars_[n] = toolbar;
	}

	current_ = toolbars_.find( n);
	active_toolbar_->setContents( current_->second);
	active_node_changed();
    }
    else
    {
	if( view_mode() == view_active_node)
	    image_view()->delete_texture();
    }

    image_view()->update();
}

void viewer_impl::active_node_changed()
{
    if( view_mode() != view_active_node)
    {
	image_view()->update();
	return;
    }

    if( !user_interface_t::Instance().active_node())
	return;

    if(!(window_->isVisible()) || !autoupdate_)
    {
	pending_update_ = true;
	image_view()->update();
	return;
    }

    node_t *current = user_interface_t::Instance().active_node();

    document_t::Instance().composition().set_result_node( current);

    render::render_context_t context = document_t::Instance().composition().current_context( render::interface_render);

    if( mblur_btn_->isChecked())
        context.motion_blur_shutter_factor = 1;
    else
        context.motion_blur_shutter_factor = 0;

    render::renderer_t renderer( context);
    renderer.render();

    image_view()->load_texture( current);
    image_view()->update();
    inspector_t::Instance().update();
    pending_update_ = false;
}

void viewer_impl::set_context_node( node_t *n)
{
    context_connection_.disconnect();

    if(n != 0)
    {
	context_connection_ = n->changed.connect( boost::bind(&viewer_impl::context_node_changed, this));
	context_node_changed();
    }
    else
    {
	if( view_mode() == view_context_node)
	{
	    image_view()->delete_texture();
	    image_view()->update();
	}
    }
}

void viewer_impl::context_node_changed()
{
    if( view_mode() != view_context_node)
    {
	image_view()->update();
	return;
    }

    if( !user_interface_t::Instance().context_node())
	return;

    if(!(window_->isVisible()) || !autoupdate_)
    {
	pending_update_ = true;
	image_view()->update();
	return;
    }

    node_t *current = user_interface_t::Instance().context_node();
    document_t::Instance().composition().set_result_node( current);

    render::render_context_t context = document_t::Instance().composition().current_context( render::interface_render);

    if( mblur_btn_->isChecked())
        context.motion_blur_shutter_factor = 1;
    else
        context.motion_blur_shutter_factor = 0;

    render::renderer_t renderer( context);
    renderer.render();

    image_view()->load_texture( current);
    image_view()->update();
    inspector_t::Instance().update();
    pending_update_ = false;
}

void viewer_impl::node_released( node_t *n)
{
    std::map<node_t*, QWidget*>::iterator it( toolbars_.find( n));

    if( it != toolbars_.end())
    {
	delete it->second;
	toolbars_.erase( n);
    }
}

void viewer_impl::frame_changed()
{
    if( view_mode() == view_active_node)
    {
	if( user_interface_t::Instance().active_node())
	    active_node_changed();
    }
    else
    {
	if( user_interface_t::Instance().context_node())
	    context_node_changed();
    }
}

// slots
void viewer_impl::autoupdate_toggle( bool state)
{
    autoupdate_ = state;

    if(autoupdate_ && pending_update_)
    {
	if( view_mode() == view_active_node)
	    active_node_changed();
	else
	    context_node_changed();
    }
}

void viewer_impl::mblur_toggle( bool state)
{
    if( view_mode() == view_active_node)
    {
	if( user_interface_t::Instance().active_node())
	    active_node_changed();
    }
    else
    {
	if( user_interface_t::Instance().context_node())
	    context_node_changed();
    }
}

void viewer_impl::checks_toggle( bool state)
{
    image_view()->show_checks_texture( state);
    image_view()->update();
}

void viewer_impl::roi_toggle( bool state) {}

void viewer_impl::overlay_toggle( bool state)
{
    image_view()->show_overlay( state);
    image_view()->update();
}

void viewer_impl::change_view( int index)
{
    if( view_mode() != index)
    {
	set_view_mode( (view_node_t) index);
	if( view_mode() == view_active_node)
	{
	    if( user_interface_t::Instance().active_node())
		    active_node_changed();
	    else
	    {
		image_view()->delete_texture();
		image_view()->update();
	    }
	}
	else
	{
	    if( user_interface_t::Instance().context_node())
		    context_node_changed();
	    else
	    {
		image_view()->delete_texture();
		image_view()->update();
	    }
	}
    }
}

void viewer_impl::set_resolution( int index)
{
    resolution_combo_->setCurrentIndex( index-1);
}

void viewer_impl::change_resolution( int index)
{
    int subsample = index + 1;

    document_t::Instance().composition().set_subsample( subsample);

    if( view_mode() == view_active_node)
    {
	if( user_interface_t::Instance().active_node())
	    active_node_changed();
    }
    else
    {
	if( user_interface_t::Instance().context_node())
	    context_node_changed();
    }
}

void viewer_impl::change_channels( int index)
{
    image_view()->set_view_mode( (image_view_mode) index);
    image_view()->update();
}

void viewer_impl::change_exposure( double d)
{
    image_view()->set_exposure( d);
    image_view()->update();
}

void viewer_impl::normal_range_toggle( bool state)
{
    image_view()->set_normal_range( state);
    exposure_input_->setEnabled( !state);
    image_view()->update();
}

void viewer_impl::set_status( const std::string& text)
{
    status_->setText( QString::fromStdString( text));
}

} // namespace
} // namespace
