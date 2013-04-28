// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_NODE_HPP
#define	RAMEN_IMAGE_INPUT_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<boost/optional.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{

class image_input_node_t : public image_node_t
{
public:

    image_input_node_t();
    image_input_node_t( const boost::filesystem::path& path, bool sequence = true);

    void param_changed( param_t *p);

    imageio::reader_t *reader() const { return reader_.get();}

    void set_channels( const std::string& red, const std::string& green,
			const std::string& blue, const std::string& alpha);

protected:

    image_input_node_t( const image_input_node_t& other);
    void operator=( const image_input_node_t&);

private:

    virtual node_t *do_clone() const { return new image_input_node_t( *this);}

    virtual void do_create_params();

    virtual void do_set_frame( float f);

    virtual bool do_is_valid() const;
    virtual void do_calc_hash( const render::render_context_t& context);

    virtual void do_calc_domain( const render::render_context_t& context);
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_subsample_areas( const render::render_context_t& context);
    virtual void do_process( const render::render_context_t& context);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    Imath::Box2i real_defined_;
    float time_;
    boost::filesystem::path fname_;
    std::auto_ptr<imageio::reader_t> reader_;
};

} // namespace

#endif
