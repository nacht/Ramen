// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrSpinBox.hpp>

#include<limits>

#include<adobe/algorithm/clamp.hpp>

QrSpinBox::QrSpinBox( QWidget *parent) : QrAbstractSpinBox( parent)
{
    lo_ = -std::numeric_limits<int>::max();
    hi_ =  std::numeric_limits<int>::max();
    value_ = 0;
    setLineEditValue( 0);
}

void QrSpinBox::setMinimum( int lo) { lo_ = lo;}
void QrSpinBox::setMaximum( int hi) { hi_ = hi;}

void QrSpinBox::setRange( int lo, int hi)
{
    assert( lo <= hi);
    lo_ = lo;
    hi_ = hi;
    // TODO: should we clamp value_ here?
}

void QrSpinBox::setValue( int v)
{
    value_ = adobe::clamp( v, lo_, hi_);
    setLineEditValue( value_);
    valueChanged( value_);
}

void QrSpinBox::calculatorSetValue( double v)
{
    setValue( v);
    calculatorValueChanged( value_);
}

void QrSpinBox::emitValueChangedSignal() { valueChanged( value_);}

void QrSpinBox::mouseDragged( int xoffset, bool notify)
{
    if( notify)
    {
	setValue( value_ + xoffset);
	spinBoxMoved( value());
    }
    else
    {
	blockSignals( true);
	setValue( value_ + xoffset);
	blockSignals( false);
    }
}
