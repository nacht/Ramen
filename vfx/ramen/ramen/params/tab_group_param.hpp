// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TAB_GROUP_PARAM_HPP
#define	RAMEN_TAB_GROUP_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<QPointer>

class QTabWidget;

namespace ramen
{

class tab_group_param_t : public composite_param_t
{
    Q_OBJECT

public:

    tab_group_param_t();
    explicit tab_group_param_t( const std::string& name);

protected:

    tab_group_param_t( const tab_group_param_t& other);
    void operator=( const tab_group_param_t& other);

private:

    virtual param_t *do_clone() const { return new tab_group_param_t( *this);}

    virtual QWidget *do_create_widgets();
    virtual void do_enable_widgets( bool e);

    virtual void do_create_tracks( anim::track_t *parent);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    QPointer<QTabWidget> tab_;
};

} // namespace

#endif	/* _tab_group_param_HPP */
