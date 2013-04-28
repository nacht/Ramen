// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SEPARATOR_PARAM_HPP
#define	RAMEN_SEPARATOR_PARAM_HPP

#include<ramen/params/param.hpp>

namespace ramen
{

class separator_param_t : public param_t
{
    Q_OBJECT
    
public:

    separator_param_t();
    explicit separator_param_t( const std::string& name);

protected:

    separator_param_t( const separator_param_t& other);
    void operator=( const separator_param_t& other);

private:

    virtual param_t *do_clone() const { return new separator_param_t( *this);}

    virtual QWidget *do_create_widgets();

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif	/* _POPUP_PARAM_HPP */
