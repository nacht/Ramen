// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QR_DOUBLESPINBOX_HPP
#define	RAMEN_QR_DOUBLESPINBOX_HPP

#include<ramen/Qr/QrAbstractSpinBox.hpp>

class QrDoubleSpinBox : public QrAbstractSpinBox
{
    Q_OBJECT

public:

    QrDoubleSpinBox( QWidget *parent = 0);

    void setMinimum( double lo);
    void setMaximum( double hi);
    void setRange( double lo, double hi);

    double step() const	    { return step_;}
    void setStep( double s) { step_ = s;}

    double value() const { return value_;}
    void setValue( double v);

protected:

    virtual void emitValueChangedSignal();
    virtual void mouseDragged( int xoffset, bool notify = true);
    virtual double doubleValue() const { return value_;}

public Q_SLOTS:

    virtual void calculatorSetValue( double v);

Q_SIGNALS:

    void valueChanged( double value);
    void calculatorValueChanged( double value);
    void spinBoxMoved( double value);

private:

    double lo_, hi_;
    double value_;

    double step_;
};

#endif	/* _QrDoubleSpinBox_HPP */
