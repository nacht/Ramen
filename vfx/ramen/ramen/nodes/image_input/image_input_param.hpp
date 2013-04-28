// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_PARAM_HPP
#define	RAMEN_IMAGE_INPUT_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<vector>

#include<boost/tuple/tuple.hpp>
#include<boost/tuple/tuple_io.hpp>

#include<QPointer>
#include<QCheckBox>
#include<QLineEdit>
#include<QComboBox>

#include<ramen/Qr/QrSpinBox.hpp>
#include<ramen/util/path_sequence.hpp>

namespace ramen
{

class image_input_param_t : public static_param_t
{
    Q_OBJECT

public:

    image_input_param_t();
    explicit image_input_param_t( const std::string& name);
    image_input_param_t( const std::string& name, const boost::filesystem::path& p, bool sequence = true);

    boost::filesystem::path path_for_frame( float f) const;
    boost::tuple<int,int,int,int> channels() const;

    void set_channels( const std::string& red, const std::string& green,
			const std::string& blue, const std::string& alpha);

    std::vector<std::string> channel_list() const;

protected:

    image_input_param_t( const image_input_param_t& other);
    void operator=( const image_input_param_t& other);

private Q_SLOTS:

    void slip_changed( int s);
    void lock_pushed( int state);
    void select_pushed();
    void channel_picked( int index);

private:

    virtual param_t *do_clone() const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();

    void update_info();

    void update_popup( QComboBox *popup, const std::vector<std::string>& ch_list, int value);

    path_sequence_t sequence_for_path( const boost::filesystem::path& p) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    QPointer<QLineEdit> input_, info_;
    QPointer<QrSpinBox> slip_;
    QPointer<QCheckBox> lock_;
    QPointer<QComboBox> red_combo_, green_combo_, blue_combo_, alpha_combo_;
};

} // namespace

#endif	/* _IMAGE_INPUT_PARAM_HPP */
