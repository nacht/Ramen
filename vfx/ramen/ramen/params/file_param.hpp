// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FILE_PARAM_HPP
#define	RAMEN_FILE_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<boost/filesystem/path.hpp>

#include<QPointer>

class QLineEdit;
class QPushButton;

namespace ramen
{

class file_param_t : public static_param_t
{
    Q_OBJECT
    
public:

    file_param_t();
    explicit file_param_t( const std::string& name);

    void set_default_value( const boost::filesystem::path& p);

    void set_value( const boost::filesystem::path& p);

    void set_is_input( bool b) { is_input_ = b;}

    bool empty() const;

    std::string extension() const;
    void set_extension( const std::string& ext);

protected:

    file_param_t( const file_param_t& other);
    void operator=( const file_param_t& other);

private:

    virtual param_t *do_clone() const { return new file_param_t( *this);}

    virtual void do_evaluate( float time) {}

    virtual std::string do_str() const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    bool is_input_;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    void update_input_text( const boost::filesystem::path& p);
    void update_input_text();

    QPointer<QLineEdit> input_;
    QPointer<QPushButton> button_;

private Q_SLOTS:

    void select_pushed();
    void text_changed();
};

boost::filesystem::path get_absolute_path( const param_t& p);

} // namespace

ADOBE_NAME_TYPE_0( "path:filesystem:boost", boost::filesystem::path);

#endif	/* _file_param_HPP */
