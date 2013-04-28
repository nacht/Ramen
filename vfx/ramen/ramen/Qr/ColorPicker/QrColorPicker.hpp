// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_COLORPICKER_HPP
#define	QR_COLORPICKER_HPP

#include<QDialog>

#include<ramen/Qr/QrColor.hpp>

class QrSaturationValuePicker;
class QrHuePicker;
class QrExposurePicker;
class QDoubleSpinBox;

class QrColorPicker : public QDialog
{
    Q_OBJECT

public:

    QrColorPicker( QWidget *parent, const QrColor& c);

    const QrColor& color() const { return color_;}

    int exec_dialog();

Q_SIGNALS:

    void colorChanged( QrColor c);

public Q_SLOTS:

    void setHue( double h);
    void setExposure( double e);
    void setSaturationValue( double s, double v);

private:

    void updateColor();
    
    QrColor prev_color_;
    QrColor color_;

    QrSaturationValuePicker *sv_picker_;
    QrHuePicker *hue_picker_;
    QrExposurePicker *exp_picker_;

    QDoubleSpinBox *exp_spinbox_;

    QDoubleSpinBox *red_spinbox_;
    QDoubleSpinBox *green_spinbox_;
    QDoubleSpinBox *blue_spinbox_;
};

#endif
