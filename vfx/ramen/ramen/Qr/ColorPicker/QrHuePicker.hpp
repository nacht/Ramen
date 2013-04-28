// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_HUE_PICKER_HPP
#define	QR_HUE_PICKER_HPP

#include<QWidget>
#include<QImage>

class QrHuePicker : public QWidget
{
    Q_OBJECT

public:

    QrHuePicker( QWidget *parent = 0);

    QSize sizeHint() const;

    double hue() const;

Q_SIGNALS:

    void hueChanged( double hue);

public Q_SLOTS:

    void setHue( double h);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);
    
private:

    double pickHue( int y) const;

    void updateBackground();

    QImage background_;
    bool valid_background_;

    double hue_;
};

#endif
