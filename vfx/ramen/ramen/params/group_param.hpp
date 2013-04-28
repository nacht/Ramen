// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_GROUP_PARAM_HPP
#define	RAMEN_GROUP_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<QPointer>

class QGroupBox;

namespace ramen
{

class group_param_t : public composite_param_t
{
    Q_OBJECT

public:

    group_param_t();
    explicit group_param_t( const std::string& name);

protected:

    group_param_t( const group_param_t& other);
    void operator=( const group_param_t& other);

private:

    virtual param_t *do_clone() const { return new group_param_t( *this);}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QGroupBox> group_;
};

} // namespace

#endif	/* _group_param_HPP */
