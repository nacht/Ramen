// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_NODE_HPP
#define	RAMEN_OFX_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include"ofxCore.h"

#include<ramen/util/signal.hpp>

// OFX Host Support forward declarations
namespace OFX
{
namespace Host
{

namespace ImageEffect
{
    class ImageEffectPlugin;
    class Descriptor;
} // namespace

namespace Param
{
    class Descriptor;
} // namespace

} // namespace
} // namespace

namespace ramen
{

class composite_param_t;

namespace ofx
{

class image_effect_t;
class ofx_manipulator_t;
class clip_t;

struct clip_info_t
{

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & BOOST_SERIALIZATION_NVP( name);
	ar & BOOST_SERIALIZATION_NVP( index);
	ar & BOOST_SERIALIZATION_NVP( is_output);
	ar & BOOST_SERIALIZATION_NVP( is_optional);
	ar & BOOST_SERIALIZATION_NVP( tooltip);
    }

    std::string name;
    int index;
    bool is_output;
    bool is_optional;
    std::string tooltip;
};

} // ofx

class ofx_node_t : public image_node_t
{
public:

    ofx_node_t();
    ofx_node_t( OFX::Host::ImageEffect::ImageEffectPlugin *plugin);

    ~ofx_node_t();

    int index_for_clip_name( const std::string& name) const;

    const render::render_context_t& render_context() const { return context_;}

    void create_effect_instance( bool call_instance_edit = true);

    void block_param_changed_signal( bool b);

    const ofx::ofx_manipulator_t *overlay() const;

    // util
    Imath::Box2i vertical_flip_box( const Imath::Box2i& b) const;
    Imath::V2f vertical_flip_point( const Imath::V2f& p) const;

protected:

    ofx_node_t( const ofx_node_t& other);
    void operator=( const ofx_node_t& other);

private:

    friend class ofx::image_effect_t;
    friend class ofx::clip_t;

    virtual node_t *do_clone() const;

    const ofx::image_effect_t *effect() const   { return effect_;}
    ofx::image_effect_t *effect()               { return effect_;}

    void get_descriptor();

    void create_plugs();
    bool all_inputs_optional() const { return all_inputs_optional_;}

    OfxStatus clip_preferences_action();
    void do_connected( node_t *src, int port);

    // params
    virtual void do_create_params();

    void param_changed( param_t *p);
    composite_param_t *find_param( const std::string& name);

    // manipulators
    virtual void do_create_manipulators();

    virtual bool do_is_valid() const;

    // hash
    virtual void do_calc_hash( const render::render_context_t& context);

    // cache
    virtual bool is_frame_varying() const;

    // areas
    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_inputs_interest( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    // processing
    virtual void do_recursive_process( const render::render_context_t& context);
    virtual void do_process( const render::render_context_t& context);

    // serialization
    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    std::string id_;
    int major_version_;
    int minor_version_;
    std::string ofx_context_;

    OFX::Host::ImageEffect::ImageEffectPlugin *plugin_;
    OFX::Host::ImageEffect::Descriptor *descriptor_;

    std::vector<ofx::clip_info_t> clips_info_;

    ofx::image_effect_t *effect_;

    render::render_context_t context_;

    bool all_inputs_optional_;
    signal_connection_t param_changed_connection_;
};

} // namespace

#endif
