// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOTION_BLUR_PARAM_HPP
#define	RAMEN_MOTION_BLUR_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<QPointer>

class QComboBox;
class QrDoubleSpinBox;
class QrSpinBox;

namespace ramen
{

struct motion_blur_info_t
{
    enum filter_type
    {
        box_filter = 0,
        triangle_filter,
        cubic_filter
    };

    motion_blur_info_t() : shutter( 0), samples( 1), shutter_offset(0), filter( box_filter) {}

    bool operator==( const motion_blur_info_t& other) const;
    bool operator!=( const motion_blur_info_t& other) const;

    template<class Archive>
    void serialize( Archive& ar, unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP( samples);
        ar & BOOST_SERIALIZATION_NVP( shutter);
        ar & BOOST_SERIALIZATION_NVP( shutter_offset);
        ar & BOOST_SERIALIZATION_NVP( filter);
    }

    std::size_t samples;
    float shutter;
    float shutter_offset;
    filter_type filter;
};

std::ostream& operator<<( std::ostream& os, const motion_blur_info_t& mb);

class motion_blur_param_t : public static_param_t
{
    Q_OBJECT

public:

    struct loop_data_t
    {
        loop_data_t( float time, int samples, float shutter, float offset, motion_blur_info_t::filter_type f);

        float weight_for_time( float t) const;

        int num_samples;
        float start_time;
        float center_time;
        float end_time;
        float time_step;
        motion_blur_info_t::filter_type filter;
    };

    motion_blur_param_t();
    explicit motion_blur_param_t( const std::string& name);

    loop_data_t loop_data( float time, int extra_samples, float shutter_factor) const;

protected:

    motion_blur_param_t( const motion_blur_param_t& other);
    void operator=( const motion_blur_param_t& other);

private Q_SLOTS:

    void samples_changed( int v);
    void shutter_changed( double v);
    void filter_changed( int index);

private:

    virtual param_t *do_clone() const;

    virtual std::string do_str() const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    QPointer<QrSpinBox> samples_;
    QPointer<QrDoubleSpinBox> shutter_, shutter_off_;
    QPointer<QComboBox> filter_;
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "motion_blur_info_t:ramen", ramen::motion_blur_info_t);

#endif
