// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLOR_CORRECT_NODE_HPP
#define	RAMEN_COLOR_CORRECT_NODE_HPP

#include<ramen/nodes/base/pointop_node.hpp>

#include<ramen/params/composite_param.hpp>

namespace ramen
{

class color_correct_node_t : public pointop_node_t
{
public:

    color_correct_node_t();

protected:

    color_correct_node_t( const color_correct_node_t& other) : pointop_node_t( other) {}
    void operator=( const color_correct_node_t&);

protected:

    void create_params_inside_param( composite_param_t *g, const std::string& id_prefix);

private:

    node_t *do_clone() const { return new color_correct_node_t( *this);}

    virtual void do_create_params();

    std::auto_ptr<composite_param_t> create_param_tab( const std::string& name, const std::string& id,
							const std::string& id_prefix);

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

class color_correct2_node_t : public color_correct_node_t
{
public:

    color_correct2_node_t() : color_correct_node_t() {}

protected:

    color_correct2_node_t( const color_correct2_node_t& other) : color_correct_node_t( other) {}
    void operator=( const color_correct2_node_t&);

private:

    node_t *do_clone() const { return new color_correct2_node_t( *this);}

    virtual void do_create_params();

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

#endif
