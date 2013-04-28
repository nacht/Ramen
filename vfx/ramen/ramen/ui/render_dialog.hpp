// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_DIALOG_HPP
#define	RAMEN_RENDER_DIALOG_HPP

#include<loki/Singleton.h>

#include<QDialog>

class QSpinBox;
class QDoubleSpinBox;
class QComboBox;

namespace ramen
{
namespace ui
{

class render_dialog_impl : public QDialog
{
    Q_OBJECT
    
public:

    render_dialog_impl();

    int start_frame() const;
    int end_frame() const;
    int resolution() const;
    bool selected_only() const;
    int mblur_extra_samples() const;
    float mblur_shutter_factor() const;

    void set_frame_range( int lo, int hi);
    void set_any_output_selected( bool b);
    void set_mblur_settings( int samples, float factor);

private:

    friend struct Loki::CreateUsingNew<render_dialog_impl>;

    QComboBox *output_popup_;
    QComboBox *resolution_popup_;
    QSpinBox *range_start_;
    QSpinBox *range_end_;

    QSpinBox *mblur_extra_samples_;
    QDoubleSpinBox *mblur_shutter_factor_;
};

typedef Loki::SingletonHolder<render_dialog_impl> render_dialog_t;

} // namespace
} // namespace

#endif	/* _RENDER_DIALOG_HPP */
