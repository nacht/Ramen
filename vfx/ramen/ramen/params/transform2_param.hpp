// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TRANSFORM2_PARAM_HPP
#define	RAMEN_TRANSFORM2_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<boost/optional.hpp>

#include<OpenEXR/ImathMatrix.h>

namespace ramen
{

class transform2_param_t : public composite_param_t
{
    Q_OBJECT

public:

    transform2_param_t();
    explicit transform2_param_t( const std::string& name, const std::string& id);

    const param_t& center_param() const;
    param_t& center_param();

    const param_t& translate_param() const;
    param_t& translate_param();

    const param_t& scale_param() const;
    param_t& scale_param();

    const param_t& rotate_param() const;
    param_t& rotate_param();

    boost::optional<Imath::M33d> matrix_at_time( float time, int subsample = 1) const;

    boost::optional<Imath::M33d> xform_blur_matrix_at_time( float time, float t, int subsample = 1) const;

protected:

    transform2_param_t( const transform2_param_t& other);
    void operator=( const transform2_param_t& other);

private:

    virtual param_t *do_clone() const { return new transform2_param_t( *this);}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
