// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMP_SETTINGS_DIALOG_HPP
#define	RAMEN_COMP_SETTINGS_DIALOG_HPP

#include<loki/Singleton.h>

#include<QDialog>

class QrFormatWidget;

namespace ramen
{
namespace ui
{

class composition_settings_dialog_impl : public QDialog
{
    Q_OBJECT

public:

    composition_settings_dialog_impl();

    void exec_dialog();

private:

    friend struct Loki::CreateUsingNew<composition_settings_dialog_impl>;

    QrFormatWidget *format_;
};

typedef Loki::SingletonHolder<composition_settings_dialog_impl> composition_settings_dialog_t;

} // namespace
} // namespace

#endif	/* _composition_settings_DIALOG_HPP */
