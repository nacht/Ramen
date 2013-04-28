// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#ifndef RAMEN_SE_LENS_DISTORT_NODE_HPP
#define	RAMEN_SE_LENS_DISTORT_NODE_HPP

#include<ramen/nodes/base/distort_node.hpp>

namespace ramen
{

class se_lens_distort_node_t : public distort_node_t
{
public:

    se_lens_distort_node_t();

    const char *help_string() const;

protected:

    se_lens_distort_node_t( const se_lens_distort_node_t& other) : distort_node_t( other) {}
    void operator=( const se_lens_distort_node_t&);

    bool needs_full_input_frame() const { return true;}

private:

    node_t *do_clone() const { return new se_lens_distort_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::render_context_t& context);

    void generate_distort_table( float *lut, int lut_size);
    void generate_undistort_table( float *lut, int lut_size);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
