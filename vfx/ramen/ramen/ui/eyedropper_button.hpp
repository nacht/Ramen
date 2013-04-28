// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_EYEDROPPER_BUTTON_HPP
#define	RAMEN_UI_EYEDROPPER_BUTTON_HPP

#include<QPushButton>

#include<ramen/Qr/QrColor.hpp>

namespace ramen
{
namespace ui
{

class eyedropper_button_t : public QPushButton
{
    Q_OBJECT
    
public:

    eyedropper_button_t( QWidget *parent = 0);

    bool eventFilter( QObject *watched, QEvent *event);

Q_SIGNALS:

    void color_picked( const QrColor&);

private Q_SLOTS:

    void tool_selected( bool b);

private:

    QrColor value_;
};

} // namespace
} // namespace

#endif	/* _EYEDROPPER_BUTTON_HPP */
