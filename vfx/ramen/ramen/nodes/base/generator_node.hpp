// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_GENERATOR_NODE_HPP
#define RAMEN_GENERATOR_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<gil/extension/algorithm/tbb/tbb_copy.hpp>

namespace ramen
{

class generator_node_t : public image_node_t
{
public:

    generator_node_t();

    virtual bool use_cache() const { return false;}

protected:

    generator_node_t( const generator_node_t& other);
    void operator=( const generator_node_t&);

    virtual void do_create_params();

    template<class VirtualView>
    void render_virtual_view( const VirtualView& view)
    {
	Imath::Box2i area( intersect( domain(), defined()));
        boost::gil::tbb_copy_pixels( boost::gil::subimage_view( view, area.min.x, area.min.y,
								area.size().x + 1, area.size().y + 1),
					    subimage_view( area));
    }

private:

    virtual void do_calc_domain( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::generator_node_t)

#endif
