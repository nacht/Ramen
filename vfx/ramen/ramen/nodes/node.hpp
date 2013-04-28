/**
 * @file
 * @brief Base class for all nodes in the compositor.
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_HPP
#define	RAMEN_NODE_HPP

#include<vector>
#include<string>
#include<memory>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

#include<boost/serialization/access.hpp>
#include<boost/serialization/assume_abstract.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/base_object.hpp>
#include<boost/serialization/export.hpp>

#include<boost/signal.hpp>

#include<adobe/md5.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/dataflow/base_node.hpp>
#include<ramen/manipulators/manipulable.hpp>

#include<ramen/app/composition_fwd.hpp>
#include<ramen/nodes/input_plug_info.hpp>
#include<ramen/nodes/node_factory.hpp>


#include<ramen/render/render_context.hpp>
#include<ramen/image/image_buffer.hpp>

#include<ramen/undo/command.hpp>
#include<ramen/anim/track.hpp>

#include<ramen/util/md5_hash_generator.hpp>

#include<QWidget>

namespace ramen
{

/**
 * @class node_t
 * @brief Basic node
 *
 * node_t is the base class of every node in Ramen.
 *
 */

class node_t : public dataflow::base_node_t<node_t>, public manipulable_t
{    
public:

    typedef dataflow::base_node_t<node_t> dataflow_base_node_t;

    enum flag_bits
    {
	selected_bit		= 1 << 0,
	ignored_bit		= 1 << 1,
	plugin_error_bit	= 1 << 2,
        active_bit              = 1 << 3,
        context_bit             = 1 << 4,
        can_motion_blur_bit     = 1 << 5,
        cacheable_bit           = 1 << 6
    };

    node_t();
    virtual ~node_t();

    /** Makes a copy of this node
     */
    node_t *clone() const { return do_clone();}

    /** Get the node name
     */
    const std::string& name() const		{ return name_;}

    /** Set the node name
     * @param n The new name
     */
    void set_name( const std::string& n)	{ name_ = n;}

    // ui
    const Imath::V2f& location() const		{ return loc_;}
    void set_location( const Imath::V2f& p)	{ loc_ = p;}
    void offset_location( const Imath::V2f& v)	{ loc_ += v;}

    // selection
    bool selected() const;
    void select( bool state);
    void toggle_selection();

    bool plugin_error() const;
    void set_plugin_error( bool b);

    bool can_motion_blur() const;
    void set_can_motion_blur( bool b);

    bool cacheable() const;
    void set_cacheable( bool b);

    /** Return true if this node is the active node
     */
    bool is_active() const;

    /** Return true if this node is the context node
     */
    bool is_context() const;

    // plugs
    /** Adds an input plug to this node
     * @param info Info about the plug
     * @param optional If the plug is optional
     */
    void add_input_plug( const input_plug_info_t& info, bool optional = false);

    const std::vector<input_plug_info_t>& plugs_info() const	{ return plugs_info_;}
    std::vector<input_plug_info_t>& plugs_info()		{ return plugs_info_;}

    // params
    virtual void param_edit_finished();

    void domain_changed();
    void subsample_changed( int s) { do_subsample_changed( s);}

    // observer
    typedef boost::signal<void ( node_t*)>		    signal_type;
    typedef boost::signals::connection			    connection_type;

    // signals
    //! Signal called when the node needs updating
    signal_type changed;

    //! Notify observers of changes in the node
    void notify();

    //! Notify dependent nodes of changes in the node
    virtual void notify_dependents();

    signal_type params_changed;

    // connected
    bool accept_input( node_t *src, int port) const	{ return do_accept_input( src, port);}
    void connected( node_t *src, int port)		{ do_connected( src, port);}

    void input_changed( node_t *input)	{ do_input_changed( input);}

    // ignore
    bool ignored() const;
    void set_ignored( bool b);

    // time
    void set_frame( float f);

    // edit
    void begin_edit();
    void end_edit();

    void begin_context();
    void end_context();

    // valid
    bool is_valid() const;

    // undo
    std::auto_ptr<undo::command_t> create_reset_command();

    // domain
    const Imath::Box2i& domain() const	    { return domain_;}
    void set_domain( const Imath::Box2i& d);

    void calc_domain( const render::render_context_t& context);

    void recursive_calc_domain( const render::render_context_t& context);
    void recursive_calc_domain();

    const Imath::Box2i& bounds() const { return bounds_;}
    void set_bounds( const Imath::Box2i& bounds);
    void calc_bounds( const render::render_context_t& context);

    // interest
    const Imath::Box2i& interest() const   { return interest_;}
    void clear_interest();
    void set_interest( const Imath::Box2i& roi);
    void add_interest( const Imath::Box2i& roi);
    void calc_inputs_interest( const render::render_context_t& context);

    // defined
    const Imath::Box2i& defined() const	    { return defined_;}
    void set_defined( const Imath::Box2i& b);
    void calc_defined( const render::render_context_t& context);
    
    // subsample
    void subsample_areas( const render::render_context_t& context);

    // hash
    void clear_hash();

    const md5_hash_generator_t& hash_generator() const { return hash_gen_;}
    md5_hash_generator_t& hash_generator() { return hash_gen_;}

    void append_to_hash( const std::string& str);
    std::string hash_str() const;

    bool include_input_in_hash( int i) const { return do_include_input_in_hash( i);}
    void calc_hash( const render::render_context_t& context);

    const md5_hash_generator_t::digest_type& digest();

    // cache.
    // a node is frame varying if it produces different images
    // for the same inputs and parameters. For example, a node that
    // uses random numbers.
    virtual bool is_frame_varying() const;

    // if the node is not expensive to compute like simple color corrections,
    // premultiply, ..., then it can return false here and save a bit of memory.
    virtual bool use_cache() const;

    // images
    image::image_buffer_t image() { return image_;}
    void set_image( image::image_buffer_t img) { image_ = img;}
    bool image_empty() const { return image_.empty();}

    void alloc_image();
    void release_image();

    image::image_view_t image_view();
    image::const_image_view_t const_image_view() const;

    image::image_view_t subimage_view( int x, int y, int w, int h);
    image::image_view_t subimage_view( const Imath::Box2i& area);

    image::const_image_view_t const_subimage_view( int x, int y, int w, int h) const;
    image::const_image_view_t const_subimage_view( const Imath::Box2i& area) const;

    // processing
    void recursive_process( const render::render_context_t& context);
    void process( const render::render_context_t& context);

    // animation
    void create_tracks( anim::track_t *root);

    // user interface
    virtual const char *help_string() const { return 0;}

    void draw( ui::image_view_t& view) const;

    QWidget *create_toolbar() { return do_create_toolbar();}

protected:

    node_t( const node_t& other);
    void operator=( const node_t& other);

    void set_plug_info( std::size_t index, const input_plug_info_t& info);

    virtual void do_subsample_areas( const render::render_context_t& context);

    virtual void do_recursive_process( const render::render_context_t& context);

    // cache
    bool read_image_from_cache();
    void write_image_to_cache();

    // anim

    /** Create the animation tracks for the anim editor
     */
    virtual void do_create_tracks( anim::track_t *parent);

private:

    virtual node_t *do_clone() const = 0;

    /** Called before connecting a node to this one.
     * If this method return false, the connection is not accepted
     * @param src the node that tries to connect with this one
     * @param port destination port for the connection
     */
    virtual bool do_accept_input( node_t *src, int port) const;

    /** Called before connecting a node to this one.
     * If this method return false, the connection is not accepted
     * @param src the node that was connect with this one
     * @param port destination port for the connection
     */
    virtual void do_connected( node_t *src, int port);

    virtual void do_input_changed( node_t *input);

    /** Called when the current render scale changes */
    virtual void do_subsample_changed( int s) {}

    virtual void do_set_frame( float f) {}

    /** Called when the node becomes active */
    virtual void do_begin_edit() {}

    /** Called when the node ends being active */
    virtual void do_end_edit() {}

    /** Called when the node becomes context */
    virtual void do_begin_context() {}

    /** Called when the node ends being context */
    virtual void do_end_context() {}

    virtual bool do_is_valid() const;

    // reset
    virtual std::auto_ptr<undo::command_t> do_create_reset_command();

    /**
     * The domain is the full area of the image this node produces
     * @param context A struct with the current rendering state
     */
    virtual void do_calc_domain( const render::render_context_t& context);

    virtual void do_calc_bounds( const render::render_context_t& context);

    /**
     * The interest is the region of the image needed at this node by dependent nodes
     * @param context A struct with the current rendering state
     */
    virtual void do_calc_inputs_interest( const render::render_context_t& context);

    /**
     * The defined area is the region of the image where pixels exists.
     * It is normally the intersection of the interest and the domain areas.
     * @param context A struct with the current rendering state
     */
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual bool do_include_input_in_hash( int i) const { return true;}

    /**
     * Each node can produce a md5 hash with its current state and its inputs state.
     * It is used in the image cache.
     * @param context A struct with the current rendering state
     */
    virtual void do_calc_hash( const render::render_context_t& context) {}
    
    virtual bool do_read_image_from_cache();
    virtual void do_write_image_to_cache();

    /**
     * Process function. Here the actual image processing is done
     */
    virtual void do_process( const render::render_context_t& context);

    // ui

    virtual void do_draw( ui::image_view_t& view) const;

    /**
     * Override this to create a toolbar to install in the viewer
     */
    virtual QWidget *do_create_toolbar() { return 0;}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    // vars
    composition_t *composition_;

    std::string name_;
    boost::uint32_t flags_;
    
    Imath::V2f loc_;

    std::vector<input_plug_info_t> plugs_info_;

    bool is_valid_;

    Imath::Box2i domain_, bounds_, interest_, defined_;

    md5_hash_generator_t hash_gen_;

    image::image_buffer_t image_;
};

/**
 * Clones a node
 * @param other The node to clone.
 */
node_t* new_clone( const node_t& other);

// this is for the (future) python bindings
struct node_deleter
{
    typedef void	result_type;
    typedef node_t*	argument_type;

    void operator()( node_t *x) const
    {
	if( !x->composition())	    // if x is not in a composition
	    delete x;		    // then delete it
    }
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::node_t::dataflow_base_node_t)
BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::node_t)

#endif
