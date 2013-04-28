// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/render_dialog.hpp>

#include<limits>

#include<QVBoxLayout>
#include<QHBoxLayout>

#include<QSpinBox>
#include<QDoubleSpinBox>
#include<QPushButton>
#include<QComboBox>

#include<ramen/Qr/QrLabel.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

render_dialog_impl::render_dialog_impl() : QDialog( user_interface_t::Instance().main_window())
{
    setWindowTitle( tr( "Render Composition"));

    QHBoxLayout *horizontalLayout = new QHBoxLayout( this);
    horizontalLayout->setSpacing(10);

    QGridLayout *gridLayout = new QGridLayout();

    QrLabel *output_label_ = new QrLabel();
    output_label_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    output_label_->setText( "Outputs");
    gridLayout->addWidget( output_label_, 0, 0, 1, 1);

    output_popup_ = new QComboBox();
    output_popup_->clear();
    output_popup_->insertItems(0, QStringList() << "All" << "Selected");
    gridLayout->addWidget(output_popup_, 0, 1, 1, 2);

    QrLabel *range_label_ = new QrLabel();
    range_label_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    range_label_->setText( "Range");
    gridLayout->addWidget(range_label_, 1, 0, 1, 1);

    range_start_ = new QSpinBox();
    range_start_->setRange( -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    gridLayout->addWidget(range_start_, 1, 1, 1, 1);

    range_end_ = new QSpinBox();
    range_end_->setRange( -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    gridLayout->addWidget(range_end_, 1, 2, 1, 1);

    QrLabel *resolution_label_ = new QrLabel();
    resolution_label_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    resolution_label_->setText( "Resolution");
    gridLayout->addWidget(resolution_label_, 2, 0, 1, 1);

    resolution_popup_ = new QComboBox();
    resolution_popup_->clear();
    resolution_popup_->insertItems(0, QStringList() << "Full" << "Half" << "Third" << "Quarter");
    gridLayout->addWidget(resolution_popup_, 2, 1, 1, 2);

    QrLabel *mblur_samples_label = new QrLabel();
    mblur_samples_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    mblur_samples_label->setText( "Mblur Extra Samples");
    gridLayout->addWidget(mblur_samples_label, 3, 0, 1, 1);

    mblur_extra_samples_ = new QSpinBox();
    mblur_extra_samples_->setRange( 0, 64);
    gridLayout->addWidget(mblur_extra_samples_, 3, 1, 1, 1);

    QrLabel *mblur_shutter_label = new QrLabel();
    mblur_shutter_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    mblur_shutter_label->setText( "Shutter Factor");
    gridLayout->addWidget(mblur_shutter_label, 4, 0, 1, 1);

    mblur_shutter_factor_ = new QDoubleSpinBox();
    mblur_shutter_factor_->setRange( 0, 5);
    mblur_shutter_factor_->setValue( 1);
    gridLayout->addWidget(mblur_shutter_factor_, 4, 1, 1, 1);

    horizontalLayout->addLayout(gridLayout);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QPushButton *ok_button_ = new QPushButton();
    ok_button_->setText( "OK");
    connect( ok_button_, SIGNAL( clicked()), this, SLOT( accept()));
    verticalLayout->addWidget(ok_button_);

    QPushButton *cancel_button_ = new QPushButton();
    cancel_button_->setText( "Cancel");
    connect( cancel_button_, SIGNAL( clicked()), this, SLOT( reject()));
    verticalLayout->addWidget(cancel_button_);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout->addLayout(verticalLayout);

    horizontalLayout->setSizeConstraint( QLayout::SetFixedSize );
    setSizeGripEnabled( false);
}

int render_dialog_impl::start_frame() const	{ return range_start_->value();}
int render_dialog_impl::end_frame() const	{ return range_end_->value();}
int render_dialog_impl::resolution() const	{ return resolution_popup_->currentIndex() + 1;}
bool render_dialog_impl::selected_only() const	{ return output_popup_->currentIndex() == 1;}

int render_dialog_impl::mblur_extra_samples() const     { return mblur_extra_samples_->value();}
float render_dialog_impl::mblur_shutter_factor() const  { return mblur_shutter_factor_->value();}

void render_dialog_impl::set_frame_range( int lo, int hi)
{
    range_start_->setValue( lo);
    range_end_ ->setValue( hi);
}

void render_dialog_impl::set_any_output_selected( bool b)
{
    if( b)
	output_popup_->setEnabled( true);
    else
    {
	output_popup_->setCurrentIndex( 0);
	output_popup_->setEnabled( false);
    }
}

void render_dialog_impl::set_mblur_settings( int samples, float factor)
{
    assert( samples >= 0);
    assert( factor >= 0 && factor <= 1);
    mblur_extra_samples_->setValue( samples);
    mblur_shutter_factor_->setValue( factor);
}

} // namespace
} // namespace
