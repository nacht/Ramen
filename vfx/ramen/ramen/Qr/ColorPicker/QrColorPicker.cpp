// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/ColorPicker/QrColorPicker.hpp>

#include<cmath>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QDialogButtonBox>
#include<QLabel>
#include<QDoubleSpinBox>
#include<QLabel>

#include<ramen/Qr/ColorPicker/QrSaturationValuePicker.hpp>
#include<ramen/Qr/ColorPicker/QrHuePicker.hpp>
#include<ramen/Qr/ColorPicker/QrExposurePicker.hpp>
#include<ramen/Qr/ColorPicker/QrColorSwatch.hpp>


QrColorPicker::QrColorPicker( QWidget *parent, const QrColor& c) : QDialog( parent)
{
    prev_color_ = c;
    color_ = c;

    double h, s, v, e;
    color_.toHSV( h, s, v);

    for( int i = 0; i < QrExposurePicker::max_exposure(); ++i)
    {
	double ex = std::pow( 2.0, (double) i);
	double x = v / ex;

	if( x <= 1.0)
	{
	    e = i;
	    v = x;
	    break;
	}
    }

    setWindowTitle( tr( "Color Picker"));

    QVBoxLayout *vlayout = new QVBoxLayout( this);

    QHBoxLayout *hlayout = new QHBoxLayout();

    sv_picker_ = new QrSaturationValuePicker();
    sv_picker_->setHue( h);
    sv_picker_->setSaturationValue( s, v);
    connect( sv_picker_, SIGNAL(saturationValueChanged( double, double)), this, SLOT( setSaturationValue( double, double)));
    hlayout->addWidget( sv_picker_);

    hue_picker_ = new QrHuePicker();
    hue_picker_->setHue( h);
    connect( hue_picker_, SIGNAL( hueChanged( double)), sv_picker_, SLOT( setHue( double)));
    connect( hue_picker_, SIGNAL( hueChanged( double)), this, SLOT( setHue( double)));
    hlayout->addWidget( hue_picker_);

    // color swatches
    {
	QVBoxLayout *vlayout2 = new QVBoxLayout();

	QrColorSwatch *old_color_ = new QrColorSwatch( 0, prev_color_);
	vlayout2->addWidget( old_color_);

	QrColorSwatch *new_color_ = new QrColorSwatch( 0, color_);
	connect( this, SIGNAL( colorChanged( QrColor)), new_color_, SLOT( setColor( QrColor)));
	vlayout2->addWidget( new_color_);

	hlayout->addLayout( vlayout2);
    }

    vlayout->addLayout( hlayout);

    // exposure slider
    hlayout = new QHBoxLayout();

    QLabel *label = new QLabel( tr( "Exp"));
    hlayout->addWidget( label);

    exp_picker_ = new QrExposurePicker();
    exp_picker_->setExposure( e);
    hlayout->addWidget( exp_picker_);

    exp_spinbox_ = new QDoubleSpinBox();
    exp_spinbox_->setMinimum( -20);
    exp_spinbox_->setMaximum(  20);
    exp_spinbox_->setValue( e);
    hlayout->addWidget( exp_spinbox_);

    connect( exp_spinbox_, SIGNAL( valueChanged( double)), exp_picker_, SLOT( setExposure( double)));
    connect( exp_picker_,   SIGNAL( exposureChanged( double)), exp_spinbox_, SLOT( setValue( double)));
    connect( exp_spinbox_, SIGNAL( valueChanged( double)), this, SLOT( setExposure( double)));
    vlayout->addLayout( hlayout);

    // dialog buttons
    hlayout = new QHBoxLayout();

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

void QrColorPicker::setHue( double h)				{ updateColor();}
void QrColorPicker::setExposure( double e)			{ updateColor();}
void QrColorPicker::setSaturationValue( double s, double v)	{ updateColor();}

void QrColorPicker::updateColor()
{
    double e = std::pow( 2, exp_picker_->exposure());
    color_ = QrColor::fromHSV( sv_picker_->hue(), sv_picker_->saturation(), sv_picker_->value() * e);
    colorChanged( color_);
}

int QrColorPicker::exec_dialog()
{
    int result = exec();
    return result;
}
