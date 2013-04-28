// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/composition_view.hpp>

#include<sstream>
#include<iostream>

#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QKeyEvent>
#include<QMouseEvent>
#include<QPaintEvent>
#include<QResizeEvent>
#include<QHelpEvent>
#include<QPainter>
#include<QPen>
#include<QBrush>
#include<QToolTip>

#include<ramen/ui/user_interface.hpp>
#include<ramen/app/document.hpp>
#include<ramen/ui/draw_composition.hpp>
#include<ramen/undo/undo_stack.hpp>
#include<ramen/ui/composition_view_commands.hpp>
#include<ramen/ui/palette.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

composition_view_t::composition_view_t( QWidget *parent) : QWidget( parent)
{
    #ifdef __APPLE__
	font_ = QFont( "Helvetica", 12);
    #else
	font_ = QFont( "Helvetica", 10);
    #endif
    setFocusPolicy( Qt::ClickFocus);
    scroll_mode_ = false;
    first_resize_ = true;
    connect_mode_ = false;

    setMouseTracking( true);
}

composition_view_t::~composition_view_t() {}

Imath::V2f composition_view_t::screen_to_world( const Imath::V2i& p) const
{
    return viewport_.screen_to_world(p);
}

Imath::V2i composition_view_t::world_to_screen( const Imath::V2f& p) const
{
    return viewport_.world_to_screen(p);
}

void composition_view_t::place_node( node_t *n) const
{
    float u = 0.5f;
    float v = 0.5f;

    float umargin = viewport_.world().size().x / 10;
    float vmargin = viewport_.world().size().y / 10;

    Imath::V2f p( viewport_.world().min.x + umargin + ( u * ( viewport_.world().size().x  - umargin - umargin)),
		  viewport_.world().min.y + vmargin + ( v * ( viewport_.world().size().y - vmargin - vmargin)));

    n->set_location( p);
}

void composition_view_t::place_node_near_node( node_t *n, node_t *other) const
{
    // TODO: this could be much better
    Imath::V2f p( other->location());
    p.x += 20;
    p.y += 60;
    n->set_location( p);
}

bool composition_view_t::event( QEvent *event)
{
    if( event->type() == QEvent::ToolTip)
    {
	QHelpEvent *help_event = static_cast<QHelpEvent*>( event);

	std::pair<composition_t::reverse_node_iterator, int> picked;
	picked = pick( *this, document_t::Instance().composition().nodes(),
						screen_to_world( Imath::V2i( help_event->pos().x(),
										 help_event->pos().y())));

	switch( picked.second)
	{
	case no_pick:
	    QToolTip::hideText();
	break;

	case body_picked:
	{
	    std::stringstream s;
	    s << picked.first->name() << " [ " << picked.first->domain().size().x + 1 << ", "
						<< picked.first->domain().size().y + 1 << "]";

	    QToolTip::showText( help_event->globalPos(), QString::fromStdString( s.str()));
	}
	break;

	case output_picked:
	    QToolTip::showText( help_event->globalPos(), "Output");
	break;

	default:
	    QToolTip::showText( help_event->globalPos(), ( picked.first)->plugs_info()[ picked.second].tooltip().c_str());
	}

	return true;
    }
    else
	QWidget::event( event);
}

void composition_view_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Backspace:
	user_interface_t::Instance().main_window()->delete_nodes();
    break;

    case Qt::Key_Home:
	viewport_.reset();
	update();
	event->accept();
    break;

    case Qt::Key_Comma:
    {
	Imath::V2f p( screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 1.33f);
	update();
	event->accept();
    }
    break;

    case Qt::Key_Period:
    {
	Imath::V2f p( screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 0.66f);
	update();
	event->accept();
    }
    break;

    case Qt::Key_I:
    {
	// toggle ignore for all selected nodes here
	event->accept();
    }
    break;

    default:
	event->ignore();
    }
}

void composition_view_t::keyReleaseEvent( QKeyEvent *event)
{ 
    switch( event->key())
    {
    case Qt::Key_Backspace:
    case Qt::Key_Home:
    case Qt::Key_Comma:
    case Qt::Key_Period:
    case Qt::Key_I:
	event->accept();
    break;

    default:
	event->ignore();
    }
}

void composition_view_t::mouseDoubleClickEvent( QMouseEvent *event)
{
    if( last_pick_.second == body_picked)
    {
	if( event->modifiers() & Qt::ControlModifier)
	    user_interface_t::Instance().set_context_node( &(*last_pick_.first));
	else
	    user_interface_t::Instance().set_active_node( &(*last_pick_.first));

	user_interface_t::Instance().update();
    }

    event->accept();
}

void composition_view_t::mousePressEvent( QMouseEvent *event)
{
    event->accept();

    drag_handler_    = boost::function<void ( QMouseEvent *)>();
    release_handler_ = boost::function<void ( QMouseEvent *)>();
    connect_mode_ = false;
    box_pick_mode_ = false;

    push_x_ = event->x();
    push_y_ = event->y();
    last_x_ = event->x();
    last_y_ = event->y();

    scroll_mode_ = false;
    zoom_mode_ = false;

    if( event->modifiers() & Qt::AltModifier)
    {
	if( event->modifiers() & Qt::ShiftModifier)
	{
	    zoom_mode_ = true;
	    zoom_center_ = screen_to_world( Imath::V2i( push_x_, push_y_));
	    drag_handler_ = boost::bind( &composition_view_t::zoom_drag_handler, this, _1);
	    return;
	}

	scroll_mode_ = true;
	drag_handler_ = boost::bind( &composition_view_t::scroll_drag_handler, this, _1);
	return;
    }

    last_pick_ = pick( *this, document_t::Instance().composition().nodes(), screen_to_world( Imath::V2i( push_x_, push_y_)));

    if( event->modifiers() & Qt::ControlModifier)
    {
	Imath::V2f p( screen_to_world( Imath::V2i( push_x_, push_y_)));

	std::pair<composition_t::reverse_edge_iterator, bool> result( pick( *this, document_t::Instance().composition().edges(), p));

	if( result.second)
	{
	    composition_t::reverse_edge_iterator it( result.first);
	    std::auto_ptr<undo::command_t> c( new undo::disconnect_command_t( it->src, it->dst, it->port));
	    c->redo();
	    undo::undo_stack_t::Instance().push_back( c);
	    user_interface_t::Instance().update();
	    return;
	}
    }
    
    if( (last_pick_.second != no_pick) && ( last_pick_.second != body_picked))
    {
	connect_mode_ = true;
	drag_handler_	 = boost::bind( &composition_view_t::connect_drag_handler, this, _1);
	release_handler_ = boost::bind( &composition_view_t::connect_release_handler, this, _1);
    }

    // single push
    if( last_pick_.second == no_pick && !( event->modifiers() & Qt::ShiftModifier))
    {
	document_t::Instance().deselect_all();
	box_pick_mode_ = true;
	drag_handler_	 = boost::bind( &composition_view_t::box_pick_drag_handler, this, _1);
	release_handler_ = boost::bind( &composition_view_t::box_pick_release_handler, this, _1);
    }
    else
    {
	if( last_pick_.second == body_picked)
	{
	    if( !last_pick_.first->selected())
	    {
		if( !( event->modifiers() & Qt::ShiftModifier))
			document_t::Instance().deselect_all();

		last_pick_.first->toggle_selection();
		drag_handler_ = boost::bind( &composition_view_t::move_nodes_drag_handler, this, _1);
	    }
	    else
	    {
		if( event->modifiers() & Qt::ShiftModifier)
		    last_pick_.first->select( false);
		else
		    drag_handler_ = boost::bind( &composition_view_t::move_nodes_drag_handler, this, _1);
	    }
	}
    }

    user_interface_t::Instance().update();
}

void composition_view_t::mouseMoveEvent( QMouseEvent *event)
{
    if( !event->buttons())
	return;

    if( ( event->x() != last_x_) || ( event->y() != last_y_))
    {
	if( drag_handler_)
	    drag_handler_( event);

	last_x_ = event->x();
	last_y_ = event->y();
	event->accept();
    }
}

void composition_view_t::mouseReleaseEvent( QMouseEvent *event)
{
    if( release_handler_)
	release_handler_( event);

    scroll_mode_ = false;
    zoom_mode_ = false;

    event->accept();
}

void composition_view_t::scroll_drag_handler( QMouseEvent *event)
{
    viewport_.scroll( Imath::V2i( -(event->x() - last_x_), -(event->y() - last_y_)));
    update();
}

void composition_view_t::zoom_drag_handler( QMouseEvent *event)
{
    const float zoom_speed = 0.05f;
    float zoom = 1.0f + ( zoom_speed * ( event->x() - last_x_));
    //float zoomy = 1.0f + ( zoom_speed * ( event->y() - last_y_));

    viewport_.zoom( zoom_center_, zoom, zoom);
    update();
}

void composition_view_t::move_nodes_drag_handler( QMouseEvent *event)
{
    float xoffset = ( event->x() - last_x_) / ( width()  / viewport_.world().size().x);
    float yoffset = ( event->y() - last_y_) / ( height() / viewport_.world().size().y);
    Imath::V2f offset( xoffset, yoffset);

    for( composition_t::node_iterator it( document_t::Instance().composition().nodes().begin());
		    it != document_t::Instance().composition().nodes().end(); ++it)
    {
	if( it->selected())
	    it->offset_location( offset);
    }

    update();
}

void composition_view_t::connect_drag_handler( QMouseEvent *event) { update();}

void composition_view_t::connect_release_handler( QMouseEvent *event)
{
    connect_mode_ = false;

    Imath::V2f p( screen_to_world( Imath::V2i( last_x_, last_y_)));

    composition_t& comp = document_t::Instance().composition();
    std::pair<composition_t::reverse_node_iterator, int> dest = pick( *this, comp.nodes(), p);

    if( dest.second == no_pick || dest.second == body_picked)
    {
	update();
	return;
    }

    if( (last_pick_.second == output_picked && dest.second >= 0) || (last_pick_.second >= 0 && dest.second == output_picked))
    {
	node_t *src, *dst;
	int port;

	if( last_pick_.second == output_picked)
	{
	    src = &(*last_pick_.first);
	    dst = &(*dest.first);
	    port = dest.second;
	}
	else
	{
	    src = &(*dest.first);
	    dst = &(*last_pick_.first);
	    port = last_pick_.second;
	}

	if( comp.can_connect( src, dst, port) && dst->accept_input( src, port))
	{
	    std::auto_ptr<undo::command_t> c( new undo::connect_command_t( src, dst, port));
	    c->redo();
	    undo::undo_stack_t::Instance().push_back( c);
	    user_interface_t::Instance().update();
	}
	else
	    update();
    }
}

void composition_view_t::box_pick_drag_handler( QMouseEvent *event) { update();}

void composition_view_t::box_pick_release_handler( QMouseEvent *event)
{
    Imath::Box2f b( screen_to_world( Imath::V2i( push_x_, push_y_)));
    b.extendBy( screen_to_world( Imath::V2i( last_x_, last_y_)));

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( pick( *this, n, b))
	    n.toggle_selection();
    }

    box_pick_mode_ = false;
    user_interface_t::Instance().update();
}

void composition_view_t::resizeEvent( QResizeEvent *event)
{
    if( first_resize_)
    {
	first_resize_ = false;
	viewport_.reset( event->size().width(), event->size().height());
	return;
    }

    viewport_.resize( event->size().width(), event->size().height());
    event->accept();
}

void composition_view_t::paintEvent ( QPaintEvent *event)
{
    QPainter p( this);
    p.setRenderHint( QPainter::Antialiasing);
    p.setFont( font_);
    p.setPen( Qt::NoPen);

    QBrush brush;
    Imath::Color3c col( ui::palette_t::Instance()["background"]);
    brush.setColor( QColor( col.x, col.y, col.z, 255));
    brush.setStyle( Qt::SolidPattern);
    p.setBrush( brush);
    p.drawRect( 0, 0, width(), height());

    QTransform xf;
    xf.translate( viewport_.device().min.x, viewport_.device().min.y);
    xf.scale( viewport_.zoom_x(), viewport_.zoom_y());
    xf.translate( -viewport_.world().min.x, -viewport_.world().min.y);
    p.setWorldTransform( xf);

    QPen pen;
    pen.setColor( QColor( 255, 255, 255));
    pen.setWidth( 1);
    
    p.setPen( pen);
    p.setBrush( Qt::NoBrush);

    adobe::for_each( document_t::Instance().composition().edges(), boost::bind( draw_edge, boost::ref(p), _1));
    adobe::for_each( document_t::Instance().composition().nodes(), boost::bind( draw_node, boost::ref(p), _1));

    p.resetTransform();
    p.setBrush( Qt::NoBrush);

    if( box_pick_mode_)
    {
	pen.setColor( QColor( 255, 0, 0, 255));
	p.setPen( pen);
	p.drawLine( push_x_, push_y_, last_x_, push_y_);
	p.drawLine( last_x_, push_y_, last_x_, last_y_);
	p.drawLine( last_x_, last_y_, push_x_, last_y_);
	p.drawLine( push_x_, last_y_, push_x_, push_y_);
    }
    else
    {
	if( connect_mode_)
	{
	    pen.setColor( QColor( 255, 255, 255, 255));
	    p.setPen( pen);
	    p.drawLine( push_x_, push_y_, last_x_, last_y_);
	}
    }

    event->accept();
}

} // namespace
} // namespace
