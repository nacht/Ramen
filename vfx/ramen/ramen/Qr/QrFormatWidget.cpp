// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrFormatWidget.hpp>

#include<boost/assign/std/vector.hpp>
using namespace boost::assign;

#include<QLabel>
#include<QSpinBox>
#include<QComboBox>
#include<QGridLayout>

std::vector<QrFormatWidget::format_type> QrFormatWidget::presets_;

QrFormatWidget::QrFormatWidget( QWidget *parent) : QWidget( parent)
{
    if( presets_.empty())
	init_presets();

    QGridLayout *format_layout = new QGridLayout( this);
    QLabel *label = new QLabel( tr( "Presets"));
    label->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget( label, 0, 0, 1, 1);

    format_combo_ = new QComboBox();
    format_layout->addWidget( format_combo_, 0, 1, 1, 2);

    for( int i = 0; i < presets_.size(); ++i)
	format_combo_->addItem( boost::get<0>( presets_[i]).c_str());
    
    format_combo_->addItem( tr( "Custom"));
    connect( format_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( preset_picked( int)));

    label = new QLabel( tr( "Image Size"));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget(label, 1, 0, 1, 1);

    format_width_ = new QSpinBox();
    format_width_->setMinimum(32);
    format_width_->setMaximum(8192);
    format_width_->setValue(768);
    format_width_->setSuffix( "px");
    connect( format_width_, SIGNAL( valueChanged( int)), this, SLOT( width_changed( int)));
    format_layout->addWidget( format_width_, 1, 1, 1, 1);

    format_height_ = new QSpinBox();
    format_height_->setMinimum(32);
    format_height_->setMaximum(8192);
    format_height_->setValue(576);
    format_height_->setSuffix( "px");
    connect( format_height_, SIGNAL( valueChanged( int)), this, SLOT( height_changed( int)));
    format_layout->addWidget( format_height_, 1, 2, 1, 1);

    label = new QLabel( tr( "Frame Rate"));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget(label, 2, 1, 1, 1);

    format_rate_ = new QSpinBox();
    format_rate_->setMinimum(1);
    format_rate_->setMaximum(100);
    format_rate_->setValue(25);
    format_rate_->setSuffix( "fps");
    connect( format_rate_, SIGNAL( valueChanged( int)), this, SLOT( rate_changed( int)));
    format_layout->addWidget( format_rate_, 2, 2, 1, 1);
}

int QrFormatWidget::format_width() const	{ return format_width_->value();}
void QrFormatWidget::set_format_width( int x)	{ format_width_->setValue( x);}

int QrFormatWidget::format_height() const		{ return format_height_->value();}
void QrFormatWidget::set_format_height( int x) const	{ format_height_->setValue( x);}

int QrFormatWidget::frame_rate() const	    { return format_rate_->value();}
void QrFormatWidget::set_frame_rate( int x) { format_rate_->setValue( x);}

void QrFormatWidget::width_changed( int x)  { update_presets_menu();}
void QrFormatWidget::height_changed( int x) { update_presets_menu();}
void QrFormatWidget::rate_changed( int x)   { update_presets_menu();}

void QrFormatWidget::preset_picked( int x)
{
    if( x == presets_.size()) // custom
	return;

    format_width_->blockSignals( true);
    format_height_->blockSignals( true);
    format_rate_->blockSignals( true);

    format_width_->setValue( boost::get<1>( presets_[x]));
    format_height_->setValue( boost::get<2>( presets_[x]));
    format_rate_->setValue( boost::get<3>( presets_[x]));
    
    format_width_->blockSignals( false);
    format_height_->blockSignals( false);
    format_rate_->blockSignals( false);
}

void QrFormatWidget::init_presets()
{
    presets_ +=	format_type( "PAL Square", 768, 576, 25),
		format_type( "NTSC Square", 720, 540, 30),

		format_type( "HD 720, 25", 1280, 720, 25),
		format_type( "HD 720, 30", 1280, 720, 30),
		format_type( "HD 1080, 25", 1920, 1080, 25),
		format_type( "HD 1080, 30", 1920, 1080, 30),

		format_type( "Cineon Full", 3656, 2664, 24),
		format_type( "Film 2K", 2048, 1556, 24),
		format_type( "Film 4K", 4096, 3112, 24)

		// add more formats
		;
}

void QrFormatWidget::update_presets_menu()
{
    format_combo_->blockSignals( true);
    int new_selection = presets_.size(); // custom

    for( int i = 0; i < presets_.size(); ++i)
    {
	if( ( format_width_->value() == boost::get<1>( presets_[i])) &&
		( format_height_->value() == boost::get<2>( presets_[i])) &&
		( format_rate_->value() == boost::get<3>( presets_[i])))
	{
	    new_selection = i;
	}
    }

    format_combo_->setCurrentIndex( new_selection);
    format_combo_->blockSignals( false);
}
