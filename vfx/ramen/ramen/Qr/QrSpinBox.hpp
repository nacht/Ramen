// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QR_INTSPINBOX_HPP
#define	RAMEN_QR_INTSPINBOX_HPP

#include<ramen/Qr/QrAbstractSpinBox.hpp>

class QrSpinBox : public QrAbstractSpinBox
{
    Q_OBJECT

public:

    QrSpinBox( QWidget *parent = 0);

    void setMinimum( int lo);
    void setMaximum( int hi);
    void setRange( int lo, int hi);

    int value() const { return value_;}
    void setValue( int v);

protected:

    virtual void emitValueChangedSignal();
    virtual void mouseDragged( int xoffset, bool notify = true);
    virtual double doubleValue() const { return value_;}

public Q_SLOTS:

    virtual void calculatorSetValue( double v);

Q_SIGNALS:

    void valueChanged( int value);
    void calculatorValueChanged( int value);
    void spinBoxMoved( int value);

private:

    int lo_, hi_;
    int value_;
};

#endif	/* _QrSPINBOX_HPP */
