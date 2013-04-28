// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/composition_settings_dialog.hpp>

#include<QVBoxLayout>
#include<QButtonGroup>
#include<QLabel>
#include<QFrame>
#include<QDialogButtonBox>

#include<ramen/Qr/QrFormatWidget.hpp>
#include<ramen/Qr/QrLabelSeparator.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/app/document.hpp>

namespace ramen
{
namespace ui
{

composition_settings_dialog_impl::composition_settings_dialog_impl() : QDialog( user_interface_t::Instance().main_window())
{
    setWindowTitle( tr( "Composition Settings"));

    QVBoxLayout *vlayout = new QVBoxLayout( this);

    // create widgets here
    /*
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    QLabel *label = new QLabel( "Format");
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);
    hlayout->addWidget(label);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    hlayout->addWidget(line);
    vlayout->addLayout(hlayout);
     */

    QrLabelSeparator *sep = new QrLabelSeparator( "Format");
    vlayout->addWidget( sep);

    format_ = new QrFormatWidget();
    format_t f( document_t::Instance().composition().default_format());
    format_->set_format_width( f.width);
    format_->set_format_height( f.height);
    format_->set_frame_rate( document_t::Instance().composition().frame_rate());
    vlayout->addWidget( format_);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect( buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    vlayout->addWidget( buttonBox);
}

void composition_settings_dialog_impl::exec_dialog()
{
    int result = exec();

    if( result == QDialog::Accepted)
    {
	format_t f( format_->format_width(), format_->format_height());
	document_t::Instance().composition().set_default_format( f);
	document_t::Instance().composition().set_frame_rate( format_->frame_rate());
    }
}

} // namespace
} // namespace

/*

 Put here:
 ---------

    - default format
    - frames per second

    - CTL paths
 */
