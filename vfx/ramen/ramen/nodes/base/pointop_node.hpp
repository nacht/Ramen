// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_POINTOP_NODE_HPP
#define RAMEN_POINTOP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{

class pointop_node_t : public image_node_t
{
public:

    pointop_node_t();

    virtual bool use_cache() const { return false;}

protected:

    pointop_node_t( const pointop_node_t& other);
    void operator=( const pointop_node_t&);

    template<class Fun>
    void tbb_transform_pixels( const image::const_image_view_t& src, const image::image_view_t& dst, Fun f) const
    {
        boost::gil::tbb_transform_pixels( src, dst, f);
    }

private:

    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);
    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context) = 0;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::pointop_node_t)

#endif
