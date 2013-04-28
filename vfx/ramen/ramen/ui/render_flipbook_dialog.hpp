// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_FLIPBOOK_DIALOG_HPP
#define	RAMEN_RENDER_FLIPBOOK_DIALOG_HPP

#include<loki/Singleton.h>

#include<QDialog>

class QSpinBox;
class QComboBox;

namespace ramen
{
namespace ui
{

class render_flipbook_dialog_impl : public QDialog
{
    Q_OBJECT
    
public:

    render_flipbook_dialog_impl();

    int start_frame() const;
    int end_frame() const;
    int resolution() const;
    bool selected_only() const;

    void set_frame_range( int lo, int hi);

private:

    friend struct Loki::CreateUsingNew<render_flipbook_dialog_impl>;

    QComboBox *resolution_popup_;
    QSpinBox *range_start_;
    QSpinBox *range_end_;
};

typedef Loki::SingletonHolder<render_flipbook_dialog_impl> render_flipbook_dialog_t;

} // namespace
} // namespace

#endif	/* _render_flipbook_dialog_HPP */
