// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_FORMAT_WIDGET_HPP
#define QR_FORMAT_WIDGET_HPP

#include<string>
#include<vector>
#include<boost/tuple/tuple.hpp>

#include<QWidget>

class QSpinBox;
class QComboBox;

class QrFormatWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( int format_width    READ format_width   WRITE set_format_width)
    Q_PROPERTY( int format_height   READ format_height  WRITE set_format_height)
    Q_PROPERTY( int frame_rate      READ frame_rate     WRITE set_frame_rate)

public:

    QrFormatWidget( QWidget *parent = 0);

    int format_width() const;
    void set_format_width( int x);

    int format_height() const;
    void set_format_height( int x) const;

    int frame_rate() const;
    void set_frame_rate( int x);

private Q_SLOTS:

    void width_changed( int x);
    void height_changed( int x);
    void rate_changed( int x);
    void preset_picked( int x);

private:

    void init_presets();
    void update_presets_menu();

    QComboBox *format_combo_;
    QSpinBox *format_width_, *format_height_, *format_rate_;

    typedef boost::tuple<std::string, int, int, int> format_type;
    static std::vector<format_type> presets_;
};

#endif
