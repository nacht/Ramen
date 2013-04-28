// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_LABEL_HPP
#define	QR_LABEL_HPP

#include<QLabel>

class QrLabel : public QLabel
{
    Q_OBJECT

public:

    QrLabel( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QrLabel( const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0 );

private:

};

#endif	/* _QRLABEL_HPP */

