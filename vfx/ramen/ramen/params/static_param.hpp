// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_STATIC_PARAM_HPP
#define	RAMEN_STATIC_PARAM_HPP

#include<ramen/params/param.hpp>

namespace ramen
{

class static_param_t : public param_t
{
    Q_OBJECT

public:

    static_param_t();
    explicit static_param_t( const std::string& name);

    virtual std::auto_ptr<undo::command_t> create_command();
    virtual std::auto_ptr<undo::command_t> create_reset_command();

protected:

    static_param_t( const static_param_t& other);
    void operator=( const static_param_t& other);

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::static_param_t)

#endif
