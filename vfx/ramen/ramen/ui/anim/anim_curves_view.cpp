// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_curves_view.hpp>

#include<algorithm>
#include<cmath>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QTreeView>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPaintEvent>
#include<QResizeEvent>
#include<QPainter>
#include<QPen>
#include<QBrush>

#include<ramen/anim/curve.hpp>
#include<ramen/anim/track.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/draw_pick_curves.hpp>
#include<ramen/ui/anim/anim_editor_toolbar.hpp>

#include<ramen/app/document.hpp>
#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace ui
{

anim_curves_view_t::anim_curves_view_t( QWidget *parent) : QWidget( parent), first_resize_(true)
{
    setFocusPolicy( Qt::ClickFocus);
    viewport_.set_y_down( true);
    scroll_mode_ = false;
}

anim_curves_view_t::~anim_curves_view_t() {}

float anim_curves_view_t::time_scale() const	{ return viewport_.zoom_x();}
float anim_curves_view_t::value_scale() const	{ return viewport_.zoom_y();}

Imath::V2i anim_curves_view_t::world_to_screen( const Imath::V2f& p) const
{ 
    return viewport_.world_to_screen( p);
}

Imath::V2f anim_curves_view_t::screen_to_world( const Imath::V2i& p) const
{
    return viewport_.screen_to_world( p);
}

void anim_curves_view_t::frame_area( const Imath::Box2f& area)
{
    Imath::Box2f new_area( area);
    new_area.min.x -= area.size().x / 10;
    new_area.max.x += area.size().x / 10;
    new_area.min.y -= area.size().y / 10;
    new_area.max.y += area.size().y / 10;
    viewport_.reset( viewport_.device(), new_area);
}

void anim_curves_view_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Home:
	viewport_.reset();
	update();
	event->accept();
    break;

    case Qt::Key_Comma:
    {
	Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 1.33f);
	update();
	event->accept();
    }
    break;

    case Qt::Key_Period:
    {
	Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 0.66f);
	update();
	event->accept();
    }
    break;

    case Qt::Key_Backspace:
	anim_editor_t::Instance().delete_selected_keyframes();
	event->accept();
    break;

    default:
	anim_editor_t::Instance().toolbar()->tool()->key_press_event( *this, event);
    }
}

void anim_curves_view_t::keyReleaseEvent( QKeyEvent *event)
{ 
    switch( event->key())
    {
    case Qt::Key_Home:
    case Qt::Key_Comma:
    case Qt::Key_Period:
    case Qt::Key_Backspace:
	event->accept();
    break;

    default:
	anim_editor_t::Instance().toolbar()->tool()->key_release_event( *this, event);
    }
}

void anim_curves_view_t::mousePressEvent( QMouseEvent *event)
{
    push_x_ = event->x();
    push_y_ = event->y();
    last_x_ = event->x();
    last_y_ = event->y();

    scroll_mode_ = false;
    zoom_mode_ = false;
    zoom_center_ = screen_to_world( Imath::V2i( push_x_, push_y_));
    move_time_mode_ = false;

    if( event->modifiers() & Qt::AltModifier)
    {
	if( event->modifiers() & Qt::ShiftModifier)
	{
	    zoom_mode_ = true;
	    zoom_center_ = screen_to_world( Imath::V2i( push_x_, push_y_));
	}
	else
	    scroll_mode_ = true;
    }

    if( !scroll_mode_ && !zoom_mode_)
    {
	float time = document_t::Instance().composition().frame();
	Imath::V2i q( world_to_screen( Imath::V2f( time, 0)));

	if( abs( q.x - push_x_) <= 4)
	    move_time_mode_ = true;
	else
	    anim_editor_t::Instance().toolbar()->tool()->mouse_press_event( *this, event);
    }
    else
	event->accept();
}

void anim_curves_view_t::mouseMoveEvent( QMouseEvent *event)
{
    if( ( event->x() != last_x_) || ( event->y() != last_y_))
    {
	if( scroll_mode_)
	{
	    viewport_.scroll( Imath::V2i( -(event->x() - last_x_), -(event->y() - last_y_)));
	    update();
	    event->accept();
	}
	else
	{
	    if( zoom_mode_)
	    {
		const float zoom_speed = 0.05f;
		float zoomx = 1.0f + ( zoom_speed * ( event->x() - last_x_));
		float zoomy = 1.0f + ( zoom_speed * ( event->y() - last_y_));

		viewport_.zoom( zoom_center_, zoomx, zoomy);
		update();
		event->accept();
	    }
	    else
	    {
		if( move_time_mode_)
		{
		    Imath::V2i p( event->x(), event->y());
		    Imath::V2f q( screen_to_world( p));

		    if( q.x < ui::user_interface_t::Instance().start_frame())
			q.x = ui::user_interface_t::Instance().start_frame();

		    if( q.x > ui::user_interface_t::Instance().end_frame())
			q.x = ui::user_interface_t::Instance().end_frame();

		    if( q.x != ui::user_interface_t::Instance().frame())
			ui::user_interface_t::Instance().set_frame( q.x);
		}
		else
		    anim_editor_t::Instance().toolbar()->tool()->mouse_drag_event( *this, event);
	    }
	}

	last_x_ = event->x();
	last_y_ = event->y();
    }
}

void anim_curves_view_t::mouseReleaseEvent( QMouseEvent *event)
{
    if( !scroll_mode_ && !zoom_mode_ && !move_time_mode_)
	anim_editor_t::Instance().toolbar()->tool()->mouse_release_event( *this, event);
    else
	event->accept();

    scroll_mode_ = false;
    zoom_mode_ = false;
    move_time_mode_ = false;
}

void anim_curves_view_t::mouseDoubleClickEvent( QMouseEvent *event) { event->ignore();}

void anim_curves_view_t::paintEvent( QPaintEvent *event)
{
    QPainter painter( this);
    painter_ = &painter; // save the ptr for use in members
    painter.setRenderHint( QPainter::Antialiasing);
    painter.setPen( Qt::NoPen);

    // draw bg
    QBrush brush;
    brush.setColor( QColor( 102, 102, 102, 255));
    brush.setStyle( Qt::SolidPattern);
    painter.setBrush( brush);
    painter.drawRect( 0, 0, width(), height());

    draw_grid();

    // turn off antialiasing
    // painter_->setRenderHint( QPainter::Antialiasing, false);

    // draw in world space
    Imath::M33f view_xform_ = viewport_.world_to_screen_matrix();
    QMatrix qm( view_xform_[0][0], view_xform_[0][1], view_xform_[1][0], view_xform_[1][1], view_xform_[2][0], view_xform_[2][1]);
    painter.setWorldTransform( QTransform( qm));

    // draw curves
    BOOST_FOREACH( anim::curve_t *c, anim_editor_t::Instance().curves())
    {
	anim::track_t *track = anim_editor_t::Instance().find_track( c);

	QPen pen;
	pen.setColor( QColor( track->color().x, track->color().y, track->color().z));
	painter.setPen( pen);

	draw_curve( *this, *c);
    }

    // draw in screen space
    painter.setWorldTransform( QTransform());

    QPen pen;
    pen.setColor( QColor( 255, 255, 255));
    painter.setPen( pen);

    BOOST_FOREACH( const anim::curve_t *c, anim_editor_t::Instance().curves()){ draw_keyframes( *this, *c);}
    
    draw_axes();
    draw_time_bar();

    anim_editor_t::Instance().toolbar()->tool()->draw_overlay( *this);

    event->accept();
}

void anim_curves_view_t::resizeEvent( QResizeEvent *event)
{
    if( first_resize_)
    {
	viewport_.reset( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width()-1, height()-1)),
                         Imath::Box2f( Imath::V2i( 0, 0) , Imath::V2i( width()-1, height()-1)));
	first_resize_ = false;
	return;
    }

    viewport_.resize( event->size().width(), event->size().height());
    event->accept();
}

void anim_curves_view_t::draw_grid() const
{
    const int spacing = 30;

    QPen pen;
    pen.setColor( QColor( 132, 132, 132));
    pen.setWidth( 1);
    painter_->setPen( pen);

    // vertical
    {
	int nticks = std::floor( (double) width() / spacing);
	double range = nice_num( viewport_.world().max.x - viewport_.world().min.x, 0);
	double d = nice_num(range/( nticks-1), 1);
	double graphmin = std::floor( (double) viewport_.world().min.x / d) * d;
	double graphmax = std::ceil((double)  viewport_.world().max.x / d) * d;
	double nfrac = std::max( -std::floor( std::log10( (double) d)), 0.0);

	for( double x = graphmin; x < graphmax + 0.5 * d; x += d)
	{
	    Imath::V2f p( x, 0);
	    Imath::V2i q( world_to_screen( p));
	    painter_->drawLine( QPointF( q.x, 0), QPointF( q.x, height()));
	}
    }

    // horizontal
    {
	int nticks = std::floor( (double) height() / spacing);
	double range = nice_num( viewport_.world().max.y - viewport_.world().min.y, 0);
	double d = nice_num(range/( nticks-1), 1);
	double graphmin = std::floor( (double) viewport_.world().min.y / d) * d;
	double graphmax = std::ceil( (double) viewport_.world().max.y / d) * d;
	double nfrac = std::max( -std::floor( std::log10( (double) d)), 0.0);

	for( double y = graphmin; y < graphmax + 0.5 * d; y += d)
	{
	    Imath::V2f p( 0, y);
	    Imath::V2i q( world_to_screen( p));
	    painter_->drawLine( QPointF( 0, q.y), QPointF( width(), q.y));
	}
    }
}

void anim_curves_view_t::draw_axes() const
{
    const int spacing = 30;

    QPen pen;
    pen.setColor( QColor( 0, 0, 0));
    pen.setWidth( 1);
    painter_->setPen( pen);

    // horizontal
    {
	painter_->drawLine( QPointF( 0, height() - spacing), QPointF( width(), height() - spacing));

	int nticks = std::floor( (double) width() / spacing);
	double range = nice_num( viewport_.world().max.x - viewport_.world().min.x, 0);
	double d = nice_num(range/( nticks-1), 1);
	double graphmin = std::floor( (double) viewport_.world().min.x / d) * d;
	double graphmax = std::ceil( (double) viewport_.world().max.x / d) * d;
	double nfrac = std::max( -std::floor( std::log10( (double) d)), 0.0);

	for( double x = graphmin; x < graphmax + 0.5 * d; x += d)
	{
	    Imath::V2f p( x, 0);
	    Imath::V2i q( world_to_screen( p));
	    painter_->drawLine( QPointF( q.x, height() - spacing - 3), QPointF( q.x, height() - spacing + 3));
	    painter_->drawText( QPoint( q.x, height() - 10), QString::number( x));
	}
    }

    // vertical
    {
        painter_->drawLine( QPointF( spacing * 2, 0), QPointF( spacing * 2, height()));

	int nticks = std::floor( (double) height() / spacing);
	double range = nice_num( viewport_.world().max.y - viewport_.world().min.y, 0);
	double d = nice_num(range/( nticks-1), 1);
	double graphmin = std::floor( (double) viewport_.world().min.y / d) * d;
	double graphmax = std::ceil( (double) viewport_.world().max.y / d) * d;
	double nfrac = std::max( -std::floor( std::log10( (double) d)), 0.0);

	for( double y = graphmin; y < graphmax + 0.5 * d; y += d)
	{
	    Imath::V2f p( 0, y);
	    Imath::V2i q( world_to_screen( p));
	    painter_->drawLine( QPointF( spacing * 2 - 3, q.y), QPointF( spacing * 2 + 3, q.y));
	    painter_->drawText( QPointF( 20, q.y), QString::number( y));
	}
    }
}

void anim_curves_view_t::draw_time_bar() const
{
    float time = document_t::Instance().composition().frame();
    Imath::V2i q( world_to_screen( Imath::V2f( time, 0)));

    QPen pen;
    pen.setColor( QColor( 255, 0, 0));
    pen.setWidth( 2);
    painter_->setPen( pen);
    painter_->drawLine( QPoint( q.x, 0), QPoint( q.x, height()));
}

double anim_curves_view_t::nice_num( double x, bool round) const
{
    int expv = std::floor( std::log10( (double) x));
    double f = x / std::pow( 10.0, expv);		/* between 1 and 10 */
    double nf;				/* nice, rounded fraction */

    if (round)
	if (f<1.5) nf = 1.;
	else if (f<3.) nf = 2.;
	else if (f<7.) nf = 5.;
	else nf = 10.;
    else
	if (f<=1.) nf = 1.;
	else if (f<=2.) nf = 2.;
	else if (f<=5.) nf = 5.;
	else nf = 10.;
    return nf * std::pow(10.0, expv);
}

} // namespace
} // namespace

