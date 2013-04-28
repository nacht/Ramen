// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PREFERENCES_DIALOG_HPP
#define	RAMEN_PREFERENCES_DIALOG_HPP

#include<loki/Singleton.h>

#include<QDialog>

class QSpinBox;
class QrFormatWidget;

namespace ramen
{
namespace ui
{

class preferences_dialog_impl : public QDialog
{
    Q_OBJECT
    
public:

    void exec_dialog();

private:

    preferences_dialog_impl();

    QWidget *create_general_tab();
    QWidget *create_memory_tab();
    QWidget *create_display_tab();

    friend struct Loki::CreateUsingNew<preferences_dialog_impl>;

    QrFormatWidget *default_format_;

    QSpinBox *spinbox_;
};

typedef Loki::SingletonHolder<preferences_dialog_impl> preferences_dialog_t;

} // namespace
} // namespace

#endif	/* _preferences_DIALOG_HPP */
