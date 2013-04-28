// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/draw_pick_curves.hpp>

#include<boost/foreach.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QPainter>

#include<ramen/anim/curve.hpp>
#include<ramen/anim/track.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/IECoreExt/LineSegmentAlgo.hpp>

#include<ramen/anim/fwd_diff_cubic_evaluator.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/anim_curves_view.hpp>

namespace ramen
{
namespace ui
{

const float tangent_length = 20.0f;

bool inside_pick_distance( const Imath::V2i& p, const Imath::V2i& q)
{
    int dx = p.x - q.x;
    if( dx < 0) dx = -dx;

    if( dx > 5)
	return false;

    int dy = p.y - q.y;
    if( dy < 0) dy = -dy;

    if( dy > 5)
	return false;

    return ( p - q).length2() < 25;
}

bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& q)
{
    Imath::V2i pi( p.x, p.y);
    Imath::V2i qi( q.x, q.y);
    return inside_pick_distance( pi, qi);
}

int span_num_steps( const anim_curves_view_t& view, float t0, float t1)
{
    float pixel_len = ( t1 - t0) * view.time_scale();
    return adobe::clamp( (int) pixel_len / 5, 5, 50);
}

// curves
void draw_span( const anim_curves_view_t& view, const anim::keyframe_t& k0, const anim::keyframe_t& k1, float lo, float hi)
{
    // trivial reject spans outside the view
    if( k1.time() < view.viewport().world().min.x)
	return;

    if( k0.time() > view.viewport().world().max.x)
	return;

    // handle step special case
    if( k0.v1_auto_tangent() == anim::tangent_step)
    {
	view.painter()->drawLine( QPointF( k0.time(), k0.value()), QPointF( k1.time(), k0.value()));
	view.painter()->drawLine( QPointF( k1.time(), k0.value()), QPointF( k1.time(), k1.value()));
	return;
    }

    // linear segment optimization
    if( k0.v1_auto_tangent() == anim::tangent_linear && k1.v0_auto_tangent() == anim::tangent_linear)
    {
	view.painter()->drawLine( QPointF( k0.time(), k0.value()), QPointF( k1.time(), k1.value()));
	return;
    }

    // TODO: use forward differences here
    int steps = span_num_steps( view, k0.time(), k1.time());

    float h0 = k0.time();
    float v0 = k0.value();

    float h_inc = (k1.time() - h0) / (float) steps;
    float h = h0 + h_inc;

    float t_inc = 1.0f / (float) steps;
    float t = t_inc;

    anim::fwd_diff_cubic_evaluator_t eval( k0.cubic_polynomial(), t_inc);
    ++eval; // advance the first value

    for( int i = 1; i <= steps; ++i)
    {
	//float v = k0.evaluate_cubic( t);
        float v = adobe::clamp( eval(), lo, hi);
	view.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

	h0 = h;
	v0 = v;
	h += h_inc;
	t += t_inc;
        ++eval;
    }
}

void draw_curve( const anim_curves_view_t& view, const anim::curve_t& curve)
{
    if( curve.size() < 2)
	return;

    for( anim::curve_t::const_iterator it( curve.begin()); it != (curve.end()-1); ++it)
	draw_span( view, *it, *(it+1), curve.get_min(), curve.get_max());

    // draw pre
    if( view.viewport().world().min.x < curve.start_time())
    {
	switch( curve.extrapolation())
	{
	case anim::extrapolate_constant:
	    view.painter()->drawLine( QPointF( view.viewport().world().min.x, curve.keys().front().value()),
				    QPointF( curve.start_time(), curve.keys().front().value()));
	break;

	default:
	{
	    float pixel_len = ( curve.start_time() - view.viewport().world().min.x) * view.time_scale();
	    int steps = adobe::clamp( (int) pixel_len / 4, 5, 150);

	    float h0 = view.viewport().world().min.x;
	    float v0 = curve.evaluate( h0);

	    float h_inc = ( curve.start_time() - view.viewport().world().min.x) / (float) steps;
	    float h = h0 + h_inc;
	    float v;

	    for( int i = 1; i <= steps; ++i)
	    {
		v = curve.evaluate( h);
		view.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

		h0 = h;
		v0 = v;
		h += h_inc;
	    }
	}
	break;
	}
    }


    // draw post
    if( view.viewport().world().max.x > curve.end_time())
    {
	switch( curve.extrapolation())
	{
	case anim::extrapolate_constant:
	    view.painter()->drawLine( QPointF( curve.end_time(), curve.keys().back().value()),
					QPointF( view.viewport().world().max.x, curve.keys().back().value()));
	break;

	default:
	{
	    float pixel_len = ( curve.start_time() - view.viewport().world().min.x) * view.time_scale();
	    int steps = adobe::clamp( (int) pixel_len / 4, 5, 300);

	    float h0 = curve.end_time();
	    float v0 = curve.evaluate( h0);

	    float h_inc = ( view.viewport().world().max.x - curve.end_time()) / (float) steps;
	    float h = h0 + h_inc;

	    for( int i = 1; i <= steps; ++i)
	    {
		float v = curve.evaluate( h);
		view.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

		h0 = h;
		v0 = v;
		h += h_inc;
	    }
	}
	break;
	}
    }
}

// keyframes

// util
Imath::V2f left_tangent_pos( const anim_curves_view_t& view, const anim::keyframe_t& k, const Imath::V2f& p)
{
    Imath::V2f tv( -view.time_scale(), k.v0() * view.value_scale());
    tv.normalize();
    return p + ( tv * tangent_length);
}

Imath::V2f right_tangent_pos( const anim_curves_view_t& view, const anim::keyframe_t& k, const Imath::V2f& p)
{
    Imath::V2f tv( view.time_scale(), -k.v1() * view.value_scale());
    tv.normalize();
    return p + ( tv * tangent_length);
}

void tangents_masks( const anim::keyframe_t& k, const anim::keyframe_t *prev, const anim::keyframe_t *next,
		    bool& left, bool& right)
{
    left = right = true;

    if( !prev)
	left = false;
    else
    {
	if( prev->v1_auto_tangent() == anim::tangent_step)
	    left = false;
    }

    if( !next || ( k.v1_auto_tangent() == anim::tangent_step))
	right = false;
}

void draw_small_box( QPainter *painter, const Imath::V2f& p)
{
    painter->drawRect( QRectF( p.x - 3, p.y - 3, 5, 5));
}

void draw_keyframe( const anim_curves_view_t& view, const anim::keyframe_t& k, const anim::keyframe_t *prev,
					    const anim::keyframe_t *next)
{
    QPen pen;

    if( k.selected())
	pen.setColor( QColor( 255, 0, 0));
    else
	pen.setColor( QColor( 255, 255, 255));

    view.painter()->setPen( pen);

    Imath::V2f p( k.time(), k.value());
    Imath::V2i q( view.world_to_screen( p));
    Imath::V2f qf( q.x, q.y);

    draw_small_box( view.painter(), qf);

    bool draw_left_tan;
    bool draw_right_tan;

    tangents_masks( k, prev, next, draw_left_tan, draw_right_tan);

    if( draw_left_tan)
    {
	Imath::V2f qt( left_tangent_pos( view, k, qf));
	view.painter()->drawLine( QPointF( qf.x, qf.y), QPointF( qt.x, qt.y));
	draw_small_box( view.painter(), qt);
    }

    if( draw_right_tan)
    {
	Imath::V2f qt( right_tangent_pos( view, k, qf));
	view.painter()->drawLine( QPointF( qf.x, qf.y), QPointF( qt.x, qt.y));
	draw_small_box( view.painter(), qt);
    }
}

void draw_keyframes( const anim_curves_view_t& view, const anim::curve_t& curve)
{
    if( curve.empty())
	return;

    if( curve.size() == 1)
    {
	draw_keyframe( view, curve[0], 0, 0);
	return;
    }

    draw_keyframe( view, curve[0], 0, &(curve[1]));

    for( int i = 1; i < curve.size() - 1; ++i)
	draw_keyframe( view, curve[i], &(curve[i-1]), &(curve[i+1]));

    draw_keyframe( view, curve[ curve.size()-1], &(curve[ curve.size()-2]), 0);
}

// pick

bool pick_tangent( const anim_curves_view_t& view, anim::keyframe_t& k, const anim::keyframe_t *prev,
		    const anim::keyframe_t *next, const Imath::V2i& p, bool& left)
{
    Imath::V2f pf( p.x, p.y);

    Imath::V2i qi( view.world_to_screen( Imath::V2f( k.time(), k.value())));
    Imath::V2f q( qi.x, qi.y);

    bool pick_left_tan;
    bool pick_right_tan;

    tangents_masks( k, prev, next, pick_left_tan, pick_right_tan);

    if( pick_left_tan)
    {
	if( inside_pick_distance( pf, left_tangent_pos( view, k, q)))
	{
	    left = true;
	    return true;
	}
    }

    if( pick_right_tan)
    {
	if( inside_pick_distance( pf, right_tangent_pos( view, k, q)))
	{
	    left = false;
	    return true;
	}
    }

    return false;
}

anim::keyframe_t *pick_tangent( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::V2i& p, bool& left)
{
    if( curve.empty())
	return 0;

    if( curve.size() == 1)
	return 0;

    if( pick_tangent( view, curve[0], 0, &(curve[1]), p, left))
	return &(curve[0]);

    for( int i = 1; i < curve.size() - 1; ++i)
    {
	if( pick_tangent( view, curve[i], &(curve[i-1]), &(curve[i+1]), p, left))
	    return &(curve[i]);
    }

    if( pick_tangent( view, curve[ curve.size()-1], &(curve[ curve.size()-2]), 0, p, left))
	return &(curve[ curve.size()-1]);

    return 0;
}

anim::keyframe_t *pick_tangent( const anim_curves_view_t& view, const Imath::V2i& p, bool& left, anim::curve_t*& curve)
{
    BOOST_FOREACH( anim::curve_t *c, anim_editor_t::Instance().curves())
    {
	anim::keyframe_t *key = pick_tangent( view, *c, p, left);

	if( key)
	{
	    curve = c;
	    return key;
	}
    }

    return 0;
}

anim::keyframe_t *pick_keyframe( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::V2i& p)
{
    BOOST_FOREACH( anim::keyframe_t& k, curve.keys())
    {
	Imath::V2f qf( k.time(), k.value());
	Imath::V2i qi( view.world_to_screen( qf));

	if( inside_pick_distance( p, qi))
	    return &k;
    }

    return 0;
}

anim::keyframe_t *pick_keyframe( const anim_curves_view_t& view, const Imath::V2i& p)
{
    BOOST_FOREACH( anim::curve_t *c, anim_editor_t::Instance().curves())
    {
	anim::keyframe_t *key = pick_keyframe( view, *c, p);

	if( key)
	    return key;
    }

    return 0;
}

void pick_keyframes( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::Box2i& area)
{
    BOOST_FOREACH( anim::keyframe_t& k, curve.keys())
    {
	Imath::V2f p( k.time(), k.value());
	Imath::V2i q( view.world_to_screen( p));

	if( area.intersects( q))
	    k.toggle_selection();
    }
}

bool line_intersects_box( const Imath::V2i& a, const Imath::V2i& b, const Imath::Box2i& area)
{
    Imath::V2f af( a.x, a.y);
    Imath::V2f bf( b.x, b.y);
    Imath::Box2f areaf( Imath::V2f( area.min.x, area.min.y), Imath::V2f( area.max.x, area.max.y));
    return intersects( IECore::LineSegment2f( af, bf), areaf);
}

bool pick_span( const anim_curves_view_t& view, const anim::keyframe_t& k0, const anim::keyframe_t& k1,
		    const Imath::Box2i& area, const Imath::V2f& pworld, float lo, float hi)
{
    // trivial reject spans
    if( k1.time() < pworld.x)
	return false;

    if( k0.time() > pworld.x)
	return false;

    if( ( pworld.y < lo) || ( pworld.y > hi))
	return false;

    // handle step special case
    if( k0.v1_auto_tangent() == anim::tangent_step)
    {
	if( line_intersects_box( view.world_to_screen( Imath::V2f( k0.time(), k0.value())),
				view.world_to_screen( Imath::V2f( k1.time(), k0.value())), area))
	    return true;

	if( line_intersects_box( view.world_to_screen( Imath::V2f( k1.time(), k0.value())),
				view.world_to_screen( Imath::V2f( k1.time(), k1.value())), area))
	    return true;

	return false;
    }

    // linear segment optimization
    if( k0.v1_auto_tangent() == anim::tangent_linear && k1.v0_auto_tangent() == anim::tangent_linear)
    {
	if( line_intersects_box( view.world_to_screen( Imath::V2f( k0.time(), k0.value())),
				view.world_to_screen( Imath::V2f( k1.time(), k1.value())), area))
	    return true;

	return false;
    }

    int steps = span_num_steps( view, k0.time(), k1.time());

    float h0 = k0.time();
    float v0 = k0.value();

    float h_inc = (k1.time() - h0) / (float) steps;
    float h = h0 + h_inc;

    float t_inc = 1.0f / (float) steps;
    float t = t_inc;

    anim::fwd_diff_cubic_evaluator_t eval( k0.cubic_polynomial(), t_inc);
    ++eval; // advance the first value

    for( int i = 1; i <= steps; ++i)
    {
	//float v = k0.evaluate_cubic( t);
	//v = adobe::clamp( v, lo, hi);
        float v = adobe::clamp( eval(), lo, hi);

	if( line_intersects_box( view.world_to_screen( Imath::V2f( h0, v0)),
				view.world_to_screen( Imath::V2f( h, v)), area))
	    return true;

	h0 = h;
	v0 = v;
	h += h_inc;
	t += t_inc;
        ++eval;
    }

    return false;
}

bool pick_curve( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::V2i& p)
{
    if( curve.size() < 2)
	return false;

    Imath::V2f pworld( view.screen_to_world( p));
    Imath::Box2i area( Imath::V2i( p.x - 2, p.y - 2), Imath::V2i( p.x + 2, p.y + 2));

    if( pworld.x < curve.start_time())
    {
	// test pre
	switch( curve.extrapolation())
	{
	case anim::extrapolate_constant:
	    return line_intersects_box( view.world_to_screen( Imath::V2f( view.viewport().world().min.x, curve.keys().front().value())),
				view.world_to_screen( Imath::V2f( curve.start_time(), curve.keys().front().value())), area);
	break;

	default:
	{
	    float pixel_len = ( curve.start_time() - view.viewport().world().min.x) * view.time_scale();
	    int steps = adobe::clamp( (int) pixel_len / 2, 5, 300);

	    float h0 = view.viewport().world().min.x;
	    float v0 = curve.evaluate( h0);

	    float h_inc = ( curve.start_time() - view.viewport().world().min.x) / (float) steps;
	    float h = h0 + h_inc;
	    float v;

	    for( int i = 1; i <= steps; ++i)
	    {
		v = curve.evaluate( h);

		if( line_intersects_box( view.world_to_screen( Imath::V2f( h0, v0)),
					view.world_to_screen( Imath::V2f( h, v)), area))
		    return true;

		h0 = h;
		v0 = v;
		h += h_inc;
	    }
	}
	break;
	}

	return false;
    }

    if( pworld.x > curve.end_time())
    {
	switch( curve.extrapolation())
	{
	case anim::extrapolate_constant:
	    return line_intersects_box( view.world_to_screen( Imath::V2f( curve.end_time(), curve.keys().back().value())),
					view.world_to_screen( Imath::V2f( view.viewport().world().max.x, curve.keys().back().value())),
					area);

	break;

	default:
	{
	    float pixel_len = ( curve.start_time() - view.viewport().world().min.x) * view.time_scale();
	    int steps = adobe::clamp( (int) pixel_len / 2, 5, 300);

	    float h0 = curve.end_time();
	    float v0 = curve.evaluate( h0);

	    float h_inc = ( view.viewport().world().max.x - curve.end_time()) / (float) steps;
	    float h = h0 + h_inc;

	    for( int i = 1; i <= steps; ++i)
	    {
		float v = curve.evaluate( h);

		if( line_intersects_box( view.world_to_screen( Imath::V2f( h0, v0)),
					view.world_to_screen( Imath::V2f( h, v)), area))
		    return true;

		h0 = h;
		v0 = v;
		h += h_inc;
	    }
	}
	break;
	}

	return false;
    }

    for( anim::curve_t::const_iterator it( curve.begin()); it != (curve.end()-1); ++it)
    {
	if( pick_span( view, *it, *(it+1), area, pworld, curve.get_min(), curve.get_max()))
	    return true;
    }

    return false;
}

} // namespace
} // namespace
