// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_EXPOSURE_PICKER_HPP
#define	QR_EXPOSURE_PICKER_HPP

#include<QWidget>
#include<QImage>

class QrExposurePicker : public QWidget
{
    Q_OBJECT

public:

    QrExposurePicker( QWidget *parent = 0);

    QSize sizeHint() const;

    static double max_exposure();
    double exposure() const;

Q_SIGNALS:

    void exposureChanged( double e);

public Q_SLOTS:

    void setExposure( double e);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);
    
private:

    double pickExposure( int x) const;

    void updateBackground();

    QImage background_;
    bool valid_background_;

    double exposure_;
};

#endif
