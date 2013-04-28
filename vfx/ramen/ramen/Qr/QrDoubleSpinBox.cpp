// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrDoubleSpinBox.hpp>

#include<limits>

#include<adobe/algorithm/clamp.hpp>

QrDoubleSpinBox::QrDoubleSpinBox( QWidget *parent) : QrAbstractSpinBox( parent)
{
    lo_ = -std::numeric_limits<double>::max();
    hi_ =  std::numeric_limits<double>::max();
    value_ = 0;
    setLineEditValue( 0.0, 3);
    step_ = 1.0;
}

void QrDoubleSpinBox::setMinimum( double lo) { lo_ = lo;}
void QrDoubleSpinBox::setMaximum( double hi) { hi_ = hi;}

void QrDoubleSpinBox::setRange( double lo, double hi)
{
    assert( lo <= hi);
    lo_ = lo;
    hi_ = hi;
    // TODO: should we clamp value_ here?
}

void QrDoubleSpinBox::setValue( double v)
{
    value_ = adobe::clamp( v, lo_, hi_);
    setLineEditValue( value_, 3);
    valueChanged( value_);
}

void QrDoubleSpinBox::calculatorSetValue( double v)
{
    setValue( v);
    calculatorValueChanged( value_);
}

void QrDoubleSpinBox::emitValueChangedSignal() { valueChanged( value_);}

void QrDoubleSpinBox::mouseDragged( int xoffset, bool notify)
{
    if( notify)
    {
	setValue( value_ + (xoffset * step_));
	spinBoxMoved( value());
    }
    else
    {
	blockSignals( true);
	setValue( value_ + (xoffset * step_));
	blockSignals( false);
    }
}
