// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/preferences_dialog.hpp>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGridLayout>

#include<QDialogButtonBox>
#include<QTabWidget>
#include<QLabel>
#include<QSlider>
#include<QSpinBox>

#include<ramen/Qr/QrLabelSeparator.hpp>
#include<ramen/Qr/QrFormatWidget.hpp>

#include<ramen/app/preferences.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

#include "time_slider.hpp"

namespace ramen
{
namespace ui
{

preferences_dialog_impl::preferences_dialog_impl() : QDialog( user_interface_t::Instance().main_window())
{
    setWindowTitle( tr( "Preferences"));

    QVBoxLayout *vlayout = new QVBoxLayout( this);

    QTabWidget *tabs = new QTabWidget();
    tabs->addTab( create_general_tab(), tr( "General"));
    tabs->addTab( create_memory_tab(), tr( "Memory"));
    tabs->addTab( create_display_tab(), tr( "Display"));

    vlayout->addWidget( tabs);

    QLabel *label = new QLabel( tr( "Some options require restarting the app to take effect."));
    vlayout->addWidget( label);

    QHBoxLayout *hlayout = new QHBoxLayout();

    QSpacerItem *horizontal_spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout->addItem(horizontal_spacer);

    // button rows
    QDialogButtonBox *button_box = new QDialogButtonBox();
    QSizePolicy sizePolicy1( QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth( button_box->sizePolicy().hasHeightForWidth());
    button_box->setSizePolicy( sizePolicy1);
    button_box->setOrientation( Qt::Horizontal);
    button_box->setStandardButtons( QDialogButtonBox::Cancel| QDialogButtonBox::Ok);
    button_box->setCenterButtons( false);
    hlayout->addWidget( button_box);

    connect( button_box, SIGNAL( accepted()), this, SLOT( accept()));
    connect( button_box, SIGNAL( rejected()), this, SLOT( reject()));

    vlayout->addLayout( hlayout);
}

void preferences_dialog_impl::exec_dialog()
{
    int result = exec();

    if( result == QDialog::Accepted)
    {
	preferences_t::Instance().set_max_image_memory( spinbox_->value());

	preferences_t::Instance().set_default_format( default_format_->format_width(), default_format_->format_height(),
							default_format_->frame_rate());

	preferences_t::Instance().save();
    }
}

QWidget *preferences_dialog_impl::create_general_tab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *vlayout = new QVBoxLayout( page);

    QrLabelSeparator *sep = new QrLabelSeparator( tr( "Default composition format"));
    vlayout->addWidget( sep);

    default_format_ = new QrFormatWidget();
    format_t f = preferences_t::Instance().default_format();
    default_format_->set_format_width( f.width);
    default_format_->set_format_height( f.height);
    default_format_->set_frame_rate( preferences_t::Instance().frame_rate());
    vlayout->addWidget( default_format_);

    return page;
}

QWidget *preferences_dialog_impl::create_memory_tab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *vlayout = new QVBoxLayout( page);

    // ram cache size
    {
    QHBoxLayout *hlayout = new QHBoxLayout();

    QLabel *label = new QLabel( tr( "Image RAM cache size"));
    hlayout->addWidget( label);

    QSlider *slider = new QSlider( Qt::Horizontal);
    slider->setMinimum( 10);
    slider->setMaximum( 80);
    slider->setValue( preferences_t::Instance().max_image_memory());
    hlayout->addWidget( slider);

    spinbox_ = new QSpinBox();
    spinbox_->setMinimum( 10);
    spinbox_->setMaximum( 80);
    spinbox_->setValue( preferences_t::Instance().max_image_memory());
    spinbox_->setSuffix( "%");
    hlayout->addWidget( spinbox_);

    label = new QLabel( tr( "of total RAM size"));
    hlayout->addWidget( label);

    connect( spinbox_, SIGNAL(valueChanged(int)), slider,   SLOT(setValue(int)));
    connect( slider,   SIGNAL(valueChanged(int)), spinbox_, SLOT(setValue(int)));

    vlayout->addLayout( hlayout);
    }

    return page;
}

QWidget *preferences_dialog_impl::create_display_tab()
{
    QWidget *page = new QWidget();
    QVBoxLayout *vlayout = new QVBoxLayout( page);

    return page;
}

} // namespace
} // namespace
