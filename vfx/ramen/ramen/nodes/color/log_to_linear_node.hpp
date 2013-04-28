
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_LOG2LIN_NODE_HPP
#define RAMEN_LOG2LIN_NODE_HPP

#include<ramen/nodes/base/pointop_node.hpp>

#include<IECore/LinearToCineonDataConversion.h>

namespace ramen
{

class log_to_linear_node_t : public pointop_node_t
{
public:

    log_to_linear_node_t();

    virtual const char *help_string() const;

protected:

    log_to_linear_node_t(const log_to_linear_node_t& other) : pointop_node_t(other) { }
    void operator=(const log_to_linear_node_t&);

private:

    node_t *do_clone() const { return new log_to_linear_node_t(*this);}

    virtual void do_create_params();

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context);

    void init_lut( boost::uint16_t *lut, const IECore::LinearToCineonDataConversion<float,boost::uint16_t>& conv) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
