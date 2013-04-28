// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/image_output_node.hpp>

#ifndef RAMEN_IMAGE_SEQUENCE_OUTPUT_NODE_HPP
#define	RAMEN_IMAGE_SEQUENCE_OUTPUT_NODE_HPP

namespace ramen
{

class image_sequence_output_node_t : public image_output_node_t
{
public:
    
    image_sequence_output_node_t();

    virtual void notify_dependents() {}

    void write_image( const render::render_context_t& context);

protected:

    image_sequence_output_node_t( const image_sequence_output_node_t& other);
    void operator=( const image_sequence_output_node_t&);

    virtual void do_process( const render::render_context_t& context);

private:

    node_t *do_clone() const { return new image_sequence_output_node_t( *this);}

    virtual void do_create_params();

    virtual void param_changed( param_t *p);

    std::string extension_for_format( int format) const;
    void set_format_for_extension( const std::string& ext);

    virtual void do_calc_defined( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    Imath::Box2i input_defined_;
};

} // namespace

#endif
