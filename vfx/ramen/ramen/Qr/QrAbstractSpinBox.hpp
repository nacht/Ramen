// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_ABSTRACT_SPINBOX_HPP
#define	QR_ABSTRACT_SPINBOX_HPP

#include<QWidget>

class QLineEdit;

class QrAbstractSpinBox : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( bool mouseTracking READ mouseTracking WRITE setMouseTracking)

public:

    QrAbstractSpinBox( QWidget *parent = 0);

    bool mouseTracking() const	    { return mouse_tracking_;}
    void setMouseTracking( bool b)  { mouse_tracking_ = b;}

    virtual bool eventFilter( QObject *watched, QEvent *event);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    
Q_SIGNALS:

    void spinBoxPressed();
    void spinBoxReleased();

public Q_SLOTS:

    virtual void calculatorSetValue( double v) = 0;

private:

    int abs( int x) const
    {
	if( x < 0)
	    return -x;

        return x;
    }

protected:

    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseDragEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);

    //virtual void resizeEvent( QResizeEvent *event);

    virtual double doubleValue() const = 0;
    virtual void emitValueChangedSignal() = 0;
    virtual void mouseDragged( int xoffset, bool notify = true) = 0;

    void setLineEditValue( int v);
    void setLineEditValue( double v, int prec);

    QLineEdit *edit_;

    bool mouse_tracking_;
    int push_x_, push_y_;
    int gpush_x_, gpush_y_;
    int last_x_, last_y_;
    bool dragged_;
    bool first_time_drag_;
};

#endif	/* _QRABSTRACTSPINBOX_HPP */
