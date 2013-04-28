// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_CONTAINER_WIDGET_HPP
#define	QR_CONTAINER_WIDGET_HPP

#include<QStackedWidget>

class QrContainerWidget : public QStackedWidget
{
    Q_OBJECT

public:
    
    QrContainerWidget( QWidget *parent = 0);

    void setContents( QWidget *w);
    void clearContents();

private:

    QWidget *contents_;
};

#endif	/* _CONTAINER_WIDGET_HPP */
