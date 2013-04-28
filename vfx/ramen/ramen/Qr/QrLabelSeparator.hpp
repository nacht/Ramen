// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QRLABELSEPARATOR_HPP
#define	RAMEN_QRLABELSEPARATOR_HPP

#include<QWidget>
#include<QLabel>

class QrLabelSeparator : public QWidget
{
    Q_OBJECT

public:

    QrLabelSeparator( QWidget * parent = 0);
    QrLabelSeparator( const QString & text, QWidget * parent = 0);

private:

    void init();

    QLabel *label_;
};

#endif	/* _QRLABELSEPARATOR_HPP */
