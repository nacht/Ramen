// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_create_tool.hpp>

#include<set>

#include<QKeyEvent>
#include<QMouseEvent>

#include<IECore/BoxAlgo.h>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/nodes/qshape/shape.hpp>
#include<ramen/nodes/qshape/qshape_commands.hpp>
#include<ramen/nodes/qshape/qshape_node_view.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/image_view.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/user_interface.hpp>

#include<ramen/undo/undo_stack.hpp>

#include<ramen/bezier/algorithm.hpp>
#include<ramen/bezier/curve.hpp>

namespace ramen
{
namespace qshape
{

qshape_create_tool_t::qshape_create_tool_t( qshape_node_t *parent) : qshape_tool_t( parent), new_shape_(0), triple_p_(0) {}

void qshape_create_tool_t::draw_overlay( ui::image_view_t& view) const
{
    if( new_shape_)
	parent()->node_view().draw_shape( *new_shape_);
}

bool qshape_create_tool_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Escape:
	close_shape( false);
	view.update();
        return true;
    break;

    case Qt::Key_Enter:
    case Qt::Key_Return:
        close_shape();
        view.update();
        return true;
    break;
    }

    return false;
}

bool qshape_create_tool_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Escape:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        return true;
    break;
    }

    return false;
}

void qshape_create_tool_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();
    Imath::V2f p( view.screen_to_world( Imath::V2i( event->x(), event->y())));

    insert_point_mode_ = false;

    if( !new_shape_)
    {
	if( parent_->selected())
	{
	    float max_dist = 4.0f / view.pixel_scale();

            if(( IECore::closestPointInBox( p, parent_->selected()->control_points_bounding_box()) - p).length2() <= max_dist)
	    {
		float t = param_closest_to_point( parent_->selected(), p, max_dist * max_dist);

		if( t >= 0)
		{
		    insert_point_mode_ = true;

		    if( t - std::floor( t) != 0.0f)
		    {
			std::auto_ptr<undo::command_t> c( new undo::modify_qshape_command_t( parent_, true));

			insert_point( t);

			ui::anim_editor_t::Instance().recreate_tracks( parent_);
			ui::anim_editor_t::Instance().update();
			ui::anim_editor_t::Instance().update_tree();

			if( parent_->selected()->active())
			{
			    parent()->notify();
			    parent()->notify_dependents();
			}

			undo::undo_stack_t::Instance().push_back( c);
			ui::user_interface_t::Instance().update();
		    }

		    return;
		}
	    }
	}
	
	parent_->deselect_all();
        std::auto_ptr<shape_t> p( parent()->create_shape());
	new_shape_ = p.release();
        new_shape_->create_params();
	new_shape_->select( true);
	view.update();
    }

    if( !new_shape_->empty())
    {
	Imath::V2i q( push_x_, push_y_);
	Imath::V2i r( view.world_to_screen( new_shape_->front().p1()));

	int f = ( q - r).length2();

	if( f < 25)
	{
	    close_shape();
	    view.update();
	    return;
	}
    }

    new_shape_->push_back( qshape::triple_t( p));
    triple_p_ = &new_shape_->back();
    view.update();
    return;
}

void qshape_create_tool_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    if( insert_point_mode_)
	return;

    if( triple_p_)
    {
	Imath::V2f v( view.screen_to_world_dir( Imath::V2i( event->x() - last_x_, event->y() - last_y_)));
	triple_p_->move_left_tangent( -v);
	triple_p_->move_right_tangent( v);
	triple_p_->set_corner( false);
	triple_p_->set_broken( false);
	view.update();
    }

    last_x_ = event->x();
    last_y_ = event->y();
}

void qshape_create_tool_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    triple_p_ = 0;
    insert_point_mode_ = false;
}

void qshape_create_tool_t::deselect()
{
    if( new_shape_)
    {
	delete new_shape_;
	new_shape_ = 0;
    }
}

void qshape_create_tool_t::close_shape( bool success)
{
    if( new_shape_)
    {
        if( success)
        {
            new_shape_->close();

            if( new_shape_->valid())
            {
                new_shape_->set_shape_keyframe( document_t::Instance().composition().frame());
                std::auto_ptr<undo::add_qshape_command_t> c( new undo::add_qshape_command_t( parent_, new_shape_));
                c->redo();
                undo::undo_stack_t::Instance().push_back( c);
            }
            else
                delete new_shape_;
        }
        else
            delete new_shape_;

        new_shape_ = 0;
    }
}

float qshape_create_tool_t::param_closest_to_point( qshape::shape_t *s, const Imath::V2f& p, float max_distance2) const
{
    float min_dist = max_distance2;
    float min_param = -1.0f;
    int min_span = 0, span = 0;

    for( qshape::shape_t::const_iterator it( s->begin()); it != s->end() - 1; ++it, ++span)
    {
	bezier::curve_t<Imath::V2f, 3> bez( it->p1(), it->p2(), ( it+1)->p0(), ( it+1)->p1());

        Imath::V2f q( IECore::closestPointInBox( p, bez.bounding_box()));
        float d = ( p - q).length2();

        if( d > min_dist)
	    continue;

	float t;

	q = bezier::nearest_point_on_curve( bez, p, t);
	d = ( p - q).length2();

	if( d < min_dist)
	{
	    min_dist = d;
	    min_param = t;
	    min_span = span;
	}
    }

    // the last segment
    ++span;
    qshape::shape_t::const_iterator it( s->end()-1);
    bezier::curve_t<Imath::V2f, 3> bez( it->p1(), it->p2(), s->begin()->p0(), s->begin()->p1());

    Imath::V2f q( IECore::closestPointInBox( p, bez.bounding_box()));
    float d = ( p - q).length2();

    if( d <= min_dist)
    {
	float t;

	Imath::V2f q = bezier::nearest_point_on_curve( bez, p, t);
	float d = ( p - q).length2();

	if( d < min_dist)
	{
	    min_dist = d;
	    min_param = t;
	    min_span = span;
	}
    }

    return min_param + min_span;
}

void qshape_create_tool_t::insert_point( float param) const
{
    assert( parent_->selected());

    qshape::shape_t *s = parent_->selected();
    int span = std::floor( param);
    float t = param - span;

    qshape::shape_t::iterator it0, it1, it2;

    it1 = s->insert( span);

    if( it1 != s->begin())
	it0 = it1 - 1;
    else
	it0 = s->end()-1;

    if( it1 != s->end()-1)
	it2 = it1 + 1;
    else
	it2 = s->begin();

    subdivide_segment( t, &*it0, &*it1, &*it2);

    std::set<float> key_times;

    it0->keyframe_times( key_times);
    it2->keyframe_times( key_times);

    for( std::set<float>::const_iterator it( key_times.begin()); it != key_times.end(); ++it)
    {
	it0->evaluate( *it);
	it2->evaluate( *it);

	subdivide_segment( t, &*it0, &*it1, &*it2);

	it0->set_keyframe( *it);
	it1->set_keyframe( *it);
	it2->set_keyframe( *it);
    }

    parent_->selected()->evaluate( parent_->composition()->frame());
}

void qshape_create_tool_t::subdivide_segment( float param, qshape::triple_t *prev, qshape::triple_t *cur, qshape::triple_t *next) const
{
    if( prev->corner() && next->corner())
    {
	cur->set_corner( true);
	cur->set_broken( true);
        Imath::V2f q( ( next->p1() * param) + ( prev->p1() * ( 1.0f - param)));

	cur->set_p0( q);
	cur->set_p1( q);
	cur->set_p2( q);
    }
    else
    {
	cur->set_corner( false);
	cur->set_broken( false);

	bezier::curve_t<Imath::V2f, 3> curve( prev->p1(), prev->p2(), next->p0(), next->p1());
	bezier::curve_t<Imath::V2f, 3> span1, span2;

	bezier::split_curve( curve, param, span1, span2);

	prev->set_p2( span1.p[1]);

	cur->set_p0( span1.p[2]);
	cur->set_p1( span2.p[0]);
	cur->set_p2( span2.p[1]);

	next->set_p0( span2.p[2]);
    }
}

} // namespace
} // namespace
