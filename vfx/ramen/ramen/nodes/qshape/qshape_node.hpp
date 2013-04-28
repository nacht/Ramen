// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_NODE_HPP
#define	RAMEN_QSHAPE_NODE_HPP

#include<ramen/nodes/base/generator_node.hpp>

#include<memory>

#include<boost/ptr_container/ptr_vector.hpp>

#include<QPointer>

#include<ramen/nodes/qshape/qshape_node_view.hpp>
#include<ramen/nodes/qshape/qshape_create_tool.hpp>
#include<ramen/nodes/qshape/qshape_select_tool.hpp>
#include<ramen/nodes/qshape/qshape_convert_tool.hpp>

namespace ramen
{

class motion_blur_param_t;

namespace qshape
{

class triple_t;
class shape_t;
class qshape_toolbar_t;

} // namespace

class qshape_node_t : public generator_node_t
{
public:

    qshape_node_t();
    virtual ~qshape_node_t();

    virtual bool use_cache() const { return true;}

    // shapes
    std::size_t num_shapes() const { return shapes().size();}

    typedef boost::ptr_vector<qshape::shape_t>::iterator iterator;

    iterator begin()	{ return shapes().begin();}
    iterator end()	{ return shapes().end();}

    typedef boost::ptr_vector<qshape::shape_t>::const_iterator const_iterator;

    const_iterator begin() const    { return shapes().begin();}
    const_iterator end() const	    { return shapes().end();}

    boost::ptr_vector<qshape::shape_t>& shapes()                { return shapes_;}
    const boost::ptr_vector<qshape::shape_t>& shapes() const    { return shapes_;}

    std::auto_ptr<qshape::shape_t>create_shape();

    void add_shape( qshape::shape_t *s);
    qshape::shape_t *pop_back_shape();

    int index_for_shape( qshape::shape_t *s) const;
    void insert_shape( qshape::shape_t *s, int index);
    qshape::shape_t *release_shape( int index);

    void deselect_all();
    void select( qshape::shape_t *s);
    qshape::shape_t *selected() { return selected_;}

    boost::signal<void ( qshape::shape_t *)> selection_changed;

    // tools
    void pick_select_tool();
    void pick_create_tool();
    void pick_convert_tool();

    qshape::qshape_toolbar_t& toolbar();

    const qshape::node_view_t& node_view() const { return view_;}

protected:

    qshape_node_t( const qshape_node_t& other);
    void operator=( const qshape_node_t&);

private:

    virtual node_t *do_clone() const { return new qshape_node_t( *this);}

    virtual void do_create_params();
    const motion_blur_param_t *motion_blur_param() const;

    virtual void do_set_frame( float f);

    virtual void do_calc_bounds( const render::render_context_t& context);
    virtual void do_calc_defined( const render::render_context_t& context);

    virtual void do_calc_hash( const render::render_context_t& context);

    virtual void do_process( const render::render_context_t& context);
    virtual void do_process_motion_blur( const render::render_context_t& context);

    // ui
    virtual QWidget *do_create_toolbar();
    virtual void do_draw_overlay( ui::image_view_t& view) const;

    virtual bool do_key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool do_key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual bool do_mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

    // anim
    virtual void do_create_tracks( anim::track_t *parent);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    qshape::node_view_t view_;

    boost::ptr_vector<qshape::shape_t> shapes_;

    QPointer<qshape::qshape_toolbar_t> toolbar_;
    qshape::qshape_create_tool_t *create_;
    qshape::qshape_select_tool_t *select_;
    qshape::qshape_convert_tool_t *convert_;
    qshape::qshape_tool_t *tool_;
    
    qshape::shape_t *selected_;

    Imath::Box2i bounds_without_blur_;
    float max_blur_;
};

} // namespace

#endif
