// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_UTILITY_WINDOW_HPP
#define	QR_UTILITY_WINDOW_HPP

#include<QWidget>

class QrUtilityWindow : public QWidget
{
    Q_OBJECT

public:

    QrUtilityWindow();

    void toggle();

protected:

    void closeEvent( QCloseEvent *event);
};

#endif	/* _UTILITY_WINDOW_HPP */
