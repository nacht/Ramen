// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QRAMEN_APP_HPP
#define RAMEN_QRAMEN_APP_HPP

#include<QApplication>

// A subclass of QApplication that handles open file events os OSX.
// i.e. files opened by double clicking on the Finder.

class QRamenApp : public QApplication
{
    Q_OBJECT

public:

    QRamenApp( int& argc, char ** argv);

protected:

    bool event( QEvent *event);
};

#endif
