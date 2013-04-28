// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/render_flipbook_dialog.hpp>

#include<limits>

#include<QVBoxLayout>
#include<QHBoxLayout>

#include<QSpinBox>
#include<ramen/Qr/QrLabel.hpp>
#include<QPushButton>
#include<QComboBox>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

render_flipbook_dialog_impl::render_flipbook_dialog_impl() : QDialog( user_interface_t::Instance().main_window())
{
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QrLabel *range_label_;
    QrLabel *resolution_label_;
    QVBoxLayout *verticalLayout;
    QPushButton *ok_button_;
    QPushButton *cancel_button_;
    QSpacerItem *verticalSpacer;

    setWindowTitle( tr( "Render Flipbook"));

    horizontalLayout = new QHBoxLayout( this);
    horizontalLayout->setSpacing(10);

    gridLayout = new QGridLayout();

    range_label_ = new QrLabel();
    range_label_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    range_label_->setText( "Range");
    gridLayout->addWidget(range_label_, 1, 0, 1, 1);

    range_start_ = new QSpinBox();
    range_start_->setRange( -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    gridLayout->addWidget(range_start_, 1, 1, 1, 1);

    range_end_ = new QSpinBox();
    range_end_->setRange( -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    gridLayout->addWidget(range_end_, 1, 2, 1, 1);

    resolution_label_ = new QrLabel();
    resolution_label_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    resolution_label_->setText( "Resolution");
    gridLayout->addWidget(resolution_label_, 2, 0, 1, 1);

    resolution_popup_ = new QComboBox();
    resolution_popup_->clear();
    resolution_popup_->insertItems(0, QStringList() << "Full" << "Half" << "Third" << "Quarter");
    gridLayout->addWidget(resolution_popup_, 2, 1, 1, 2);

    horizontalLayout->addLayout(gridLayout);

    verticalLayout = new QVBoxLayout();
    ok_button_ = new QPushButton();
    ok_button_->setText( "OK");
    connect( ok_button_, SIGNAL( clicked()), this, SLOT( accept()));
    verticalLayout->addWidget(ok_button_);

    cancel_button_ = new QPushButton();
    cancel_button_->setText( "Cancel");
    connect( cancel_button_, SIGNAL( clicked()), this, SLOT( reject()));
    verticalLayout->addWidget(cancel_button_);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout->addLayout(verticalLayout);

    horizontalLayout->setSizeConstraint( QLayout::SetFixedSize );
    setSizeGripEnabled( false);
}

int render_flipbook_dialog_impl::start_frame() const	{ return range_start_->value();}
int render_flipbook_dialog_impl::end_frame() const	{ return range_end_->value();}
int render_flipbook_dialog_impl::resolution() const	{ return resolution_popup_->currentIndex() + 1;}

void render_flipbook_dialog_impl::set_frame_range( int lo, int hi)
{
    range_start_->setValue( lo);
    range_end_ ->setValue( hi);
}

} // namespace
} // namespace
