// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/image_view.hpp>

#include<algorithm>
#include<stdexcept>
#include<sstream>
#include<iostream>

#include<QKeyEvent>
#include<QMouseEvent>
#include<QHideEvent>
#include<QShowEvent>
#include<QMessageBox>

#include<ramen/GL/gl.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/ui/palette.hpp>
#include<ramen/ui/image_view_shaders.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/preferences.hpp>
#include<ramen/app/document.hpp>

namespace ramen
{
namespace ui
{

image_view_t::image_view_t( QWidget *parent) : QGLWidget( parent), first_time_(true)
{
    setFocusPolicy( Qt::ClickFocus);
    setMouseTracking( true);

    first_image_loaded_ = true;

    scroll_mode_ = false;
    texture_id_ = 0;

    exposure_ = 0.0f;
    gamma_ = preferences_t::Instance().display_gamma();
    mode_ = image_view_rgb;
    normal_range_ = true;

    checks_ = false;
    overlay_ = true;

    active_ = 0;
    event_accepted_by_node_ = false;

    domain_ = defined_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( 0, 0));
    valid_texture_ = false;

    alpha_program_ = alpha_shader_ = 0;
    color_program_ = color_shader_ = 0;

    dragged = first_drag = false;
}

image_view_t::~image_view_t()
{ 
    delete_texture();
    // TODO: remove the shaders here
}

void image_view_t::load_texture()
{
    makeCurrent();

    if( !texture_id_)
	glGenTextures( 1, &texture_id_);

    glBindTexture( GL_TEXTURE_2D, texture_id_);

    if( image_.width() != 0)
    {
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	image::image_view_t view( boost::gil::view( image_));
	float *ptr = (float *) boost::gil::interleaved_view_get_raw_data( view);
	glPixelStorei( GL_UNPACK_ROW_LENGTH, view.pixels().row_size() / 16);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, view.width(), view.height(), 0, GL_RGBA, GL_FLOAT, ptr);
	valid_texture_ = true;
    }
    else
    {
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 0, 0, 0, GL_RGBA, GL_FLOAT, 0);
	valid_texture_ = false;
    }

    glBindTexture( GL_TEXTURE_2D, 0);
}

void image_view_t::load_texture( node_t *n)
{
    // clear the image
    image_ = image::image_t();

    domain_  = n->domain();

    if( !n->defined().isEmpty())
    {
	defined_ = intersect( domain_, n->defined());
	valid_texture_ = !defined_.isEmpty();
    }
    else
    {
	defined_ = Imath::Box2i();
	valid_texture_ = false;
    }

    if( valid_texture_)
    {
	image_.recreate( defined_.size().x + 1, defined_.size().y + 1);
	boost::gil::copy_pixels( n->const_subimage_view( defined_), boost::gil::view( image_));

        if( first_image_loaded_)
        {
            viewport_.scroll_to_center_point( Imath::V2f( domain_.center()) * subsample());
            first_image_loaded_ = false;
        }
    }

    load_texture();
}

void image_view_t::delete_texture( void)
{
    makeCurrent();
    
    if( texture_id_)
    {
	glDeleteTextures( 1, &texture_id_);
	texture_id_ = 0;
	image_ = image::image_t();
	domain_ = defined_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( 0, 0));
    }

    valid_texture_ = false;
}

float image_view_t::subsample() const
{
    return document_t::Instance().composition().subsample();
}

void image_view_t::initializeGL()
{
    Imath::Color3c col( ui::palette_t::Instance()["background"]);
    glClearColor( col.x / 255.0f, col.y / 255.0f, col.z / 255.0f, 0);

    if( first_time_)
    {
	test_gl_extensions();
	viewport_.reset( width(), height());
	first_time_ = false;
    }

    color_shader_ = glCreateShader( GL_FRAGMENT_SHADER);
    glShaderSource( color_shader_, 1, &shaders::color_shader_source, 0);
    glCompileShader( color_shader_);
    color_program_ = glCreateProgram();
    glAttachShader( color_program_, color_shader_);
    glLinkProgram( color_program_);
    glUseProgram( color_program_);

    alpha_shader_ = glCreateShader( GL_FRAGMENT_SHADER);
    glShaderSource( alpha_shader_, 1, &shaders::alpha_shader_source, 0);
    glCompileShader( alpha_shader_);
    alpha_program_ = glCreateProgram();
    glAttachShader( alpha_program_, alpha_shader_);
    glLinkProgram( alpha_program_);
    glUseProgram( alpha_program_);

    glUseProgram( 0);

    if( image_.width() != 0)
	load_texture();
}

void image_view_t::resizeGL( int w, int h) { viewport_.resize( w, h);}

void image_view_t::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT);

    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode( GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode( GL_PROJECTION);
    glLoadIdentity();
    glViewport( 0, 0, width(), height());
    gluOrtho2D( 0, width(), height(), 0);
    glMatrixMode( GL_MODELVIEW);

    glLineWidth( 1);

    draw_checks();

    glMatrixMode( GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( viewport_.world().min.x, viewport_.world().max.x, viewport_.world().max.y, viewport_.world().min.y);
    glMatrixMode( GL_MODELVIEW);

    glScalef( subsample(), subsample(), 1);

    draw_image();

    glLoadIdentity();

    if( ( viewer_t::Instance().view_mode() == view_active_node) && active_)
	active_->draw( *this);

    // overlay
    if( active_ && overlay_)
	active_->draw_overlay( *this);
}

void image_view_t::enterEvent( QEvent *event)
{
    if( active_ && overlay_)
	active_->mouse_enter_event( *this, event);

    event->accept();
}

void image_view_t::leaveEvent( QEvent *event)
{
    if( active_ && overlay_)
	active_->mouse_leave_event( *this, event);

    event->accept();
}

void image_view_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Home:
	viewport_.reset();

        if( valid_texture_)
            viewport_.scroll_to_center_point( Imath::V2f( domain_.center()) * subsample());

	update();
	event->accept();
    break;

    case Qt::Key_Comma:
    {
	Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 2.0f);
	update();
	event->accept();
    }
    break;

    case Qt::Key_Period:
    {
	Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
	viewport_.zoom( p, 0.5f);
	update();
	event->accept();
    }
    break;

    default:
	if( active_ && overlay_)
	{
	    if( active_->key_press_event( *this, event))
		event->accept();
	    else
		event->ignore();
	}
    }
}

void image_view_t::keyReleaseEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Home:
    case Qt::Key_Comma:
    case Qt::Key_Period:
	event->accept();
    break;

    default:
	if( active_ && overlay_)
	{
	    if( active_->key_release_event( *this, event))
		event->accept();
	    else
		event->ignore();
	}
    }
}

void image_view_t::mouseDoubleClickEvent( QMouseEvent *event)	{ event->ignore();}

void image_view_t::mousePressEvent( QMouseEvent *event)
{
    event_accepted_by_node_ = false;

    screen_click_pos = Imath::V2i( event->x(), event->y());
    screen_last_mouse_pos = screen_click_pos;
    screen_current_mouse_pos = screen_click_pos;

    Imath::V2f q( screen_to_world( screen_click_pos));
    world_click_pos = q;
    world_last_mouse_pos = q;
    world_current_mouse_pos = q;

    dragged = false;
    first_drag = true;

    scroll_mode_ = false;
    zoom_mode_ = false;

    if( event->modifiers() & Qt::AltModifier)
    {
	if( event->modifiers() & Qt::ShiftModifier)
	{
	    zoom_mode_ = true;
	    zoom_center_ = world_click_pos;
	}
	else
	    scroll_mode_ = true;
    }

    if( active_ && overlay_)
    {
	if( !scroll_mode_ && !zoom_mode_)
	    event_accepted_by_node_ = active_->mouse_press_event( *this, event);
    }

    event->accept();
}

void image_view_t::mouseMoveEvent( QMouseEvent *event)
{
    screen_current_mouse_pos = Imath::V2i( event->x(), event->y());

    Imath::V2f q( screen_to_world( screen_current_mouse_pos));
    world_current_mouse_pos = q;

    if( event->buttons() == Qt::NoButton)
    {
	image::pixel_t col( color_at( screen_current_mouse_pos.x, screen_current_mouse_pos.y));

	std::stringstream s;
	s << "X = " << (int) world_current_mouse_pos.x << " Y = " << (int) world_current_mouse_pos.y <<
	    " Color = " << boost::gil::get_color( col, boost::gil::red_t()) << ", " <<
			    boost::gil::get_color( col, boost::gil::green_t()) << ", " <<
			    boost::gil::get_color( col, boost::gil::blue_t()) << ", " <<
			    boost::gil::get_color( col, boost::gil::alpha_t());

	viewer_t::Instance().set_status( s.str());

	if( active_ && overlay_)
	    active_->mouse_move_event( *this, event);

	event->accept();
	return;
    }

    if( ( screen_current_mouse_pos.x != screen_last_mouse_pos.x) || ( screen_current_mouse_pos.y != screen_last_mouse_pos.y))
    {
	if( scroll_mode_)
	{
	    viewport_.scroll( -( screen_current_mouse_pos - screen_last_mouse_pos));
	    update();
	}
	else
	{
	    if( zoom_mode_)
	    {
		const float zoom_speed = 0.05f;
		float zoom = 1.0f + ( zoom_speed * ( screen_current_mouse_pos.x - screen_last_mouse_pos.x));

		viewport_.zoom( zoom_center_, zoom, zoom);
		update();
	    }
	    else
	    {
		if( active_ && overlay_ && event_accepted_by_node_)
		    active_->mouse_drag_event( *this, event);
	    }
	}

        if( first_drag)
        {
            first_drag = false;
            dragged = true;
        }

        screen_last_mouse_pos = screen_current_mouse_pos;
        world_last_mouse_pos = world_current_mouse_pos;
    }

    event->accept();
}

void image_view_t::mouseReleaseEvent( QMouseEvent *event)
{ 
    if( active_ && overlay_ && event_accepted_by_node_)
	active_->mouse_release_event( *this, event);

    scroll_mode_ = false;
    zoom_mode_ = false;
    
    event->accept();
}

void image_view_t::test_gl_extensions()
{
    GLenum err = glewInit();

    if( GLEW_OK != err)
	application_t::Instance().fatal_error( "Video card not supported: Error initializing GLEW");

    if( !glewIsSupported("GL_VERSION_2_0"))
        application_t::Instance().fatal_error( "Video card not supported: Not supported OpenGL 2.0");

    if( !GLEW_ARB_texture_non_power_of_two)
        application_t::Instance().fatal_error( "Video card not supported: Not supported non power of two textures");

    if( !GLEW_ARB_texture_float)
        application_t::Instance().fatal_error( "Video card not supported: Not supported float textures");

    if( !GLEW_ARB_fragment_shader)
        application_t::Instance().fatal_error( "Video card not supported: Not supported fragment programs");
}

void image_view_t::show_checks_texture( bool b)	    { checks_ = b;}
void image_view_t::set_view_mode( image_view_mode m){ mode_ = m;}
void image_view_t::set_exposure( float e)	    { exposure_ = e;}
void image_view_t::set_gamma( float g)		    { gamma_ = g;}
void image_view_t::set_active_node( node_t *n)	    { active_ = n;}
void image_view_t::show_overlay( bool b)	    { overlay_ = b;}
void image_view_t::set_normal_range( bool b)	    { normal_range_ = b;}

float image_view_t::pixel_scale() const { return viewport_.zoom_x();}

Imath::V2f image_view_t::screen_to_world( const Imath::V2i& p) const
{
    return viewport_.screen_to_world(p);
}

Imath::V2i image_view_t::world_to_screen( const Imath::V2f& p) const
{
    return viewport_.world_to_screen(p);
}

Imath::Box2f image_view_t::screen_to_world( const Imath::Box2i& b) const
{
    return viewport_.screen_to_world(b);
}

Imath::Box2i image_view_t::world_to_screen( const Imath::Box2f& b) const
{
    return viewport_.world_to_screen(b);
}

Imath::V2f image_view_t::screen_to_world_dir( const Imath::V2f& v) const
{
    return viewport_.screen_to_world_dir(v);
}

image::pixel_t image_view_t::color_at( int x, int y) const
{
    if( !valid_texture_)
	return image::pixel_t( 0, 0, 0, 0);

    image::const_image_view_t view( boost::gil::const_view( image_));
    Imath::V2f p( screen_to_world( Imath::V2i( x, y)));

    p.x = ( p.x - defined_.min.x) / subsample();
    p.y = ( p.y - defined_.min.y) / subsample();

    if( p.x < 0 || p.y < 0 || (p.x >= image_.width()) || (p.y >= image_.height()))
	return image::pixel_t( 0, 0, 0, 0);

    return view( p.x, p.y);
}

void image_view_t::draw_checks() const
{
    if( !checks_ || mode_ != image_view_rgb)
	return;

    const int checks_size = 12;

    for( int j=0;j<height();j+=checks_size)
    {
	for( int i=0;i<width();i+=checks_size)
	{
	    if( i/checks_size % 2)
	    {
		if( !(j/checks_size % 2))
			glColor3f( .6, .6, .6);
		else
			glColor3f( .8, .8, .8);
	    }
	    else
	    {
		if( !(j/checks_size % 2))
			glColor3f( .8, .8, .8);
		else
			glColor3f( .6, .6, .6);
	    }

	    glBegin( GL_QUADS);
		glVertex2i( i, j);
		glVertex2i( i+checks_size, j);
		glVertex2i( i+checks_size, j+checks_size);
		glVertex2i( i, j+checks_size);
	    glEnd();
	}
    }
}

void image_view_t::draw_image()
{
    // draw black image bg
    if( texture_id_)
    {
        if( !( checks_ && mode_ == image_view_rgb))
	{
	    glColor4f( 0.0, 0.0, 0.0, 0.0);
	    glBegin( GL_QUADS);
		glVertex2i( domain_.min.x, domain_.min.y);
		glVertex2i( domain_.max.x, domain_.min.y);
		glVertex2i( domain_.max.x, domain_.max.y);
		glVertex2i( domain_.min.x, domain_.max.y);
	    glEnd();
	}
    }

    if( valid_texture_)
    {
	glEnable( GL_TEXTURE_2D);

	glBindTexture( GL_TEXTURE_2D, texture_id_);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if( checks_ && mode_ == image_view_rgb)
	{
	    glEnable( GL_BLEND);
	    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if( mode_ == image_view_alpha)
	{
	    glUseProgram( alpha_program_);
	    glUniform1i( glGetUniformLocation( alpha_program_, "tex"), 0);
	    glUniform1f( glGetUniformLocation( alpha_program_, "gamma"), 1.0f / gamma_);
	}
	else
	{
	    glUseProgram( color_program_);
	    glUniform1i( glGetUniformLocation( color_program_, "tex"), 0);
	    glUniform1f( glGetUniformLocation( color_program_, "gamma"), 1.0f / gamma_);

	    float c;

	    if( !normal_range_)
	    {
		glUniform1f( glGetUniformLocation( color_program_, "exposure"), std::pow( 2.0f, exposure_ + 2.47393f));
		c = 84.66f / 255.0f;
	    }
	    else
	    {
		glUniform1f( glGetUniformLocation( color_program_, "exposure"), 1.0f);
		c = 1.0f;
	    }

	    switch( mode_)
	    {
		case image_view_red:
		    glUniform4f( glGetUniformLocation( color_program_, "channel_mask"), c, 0.0f, 0.0f, 1.0f);
		break;

		case image_view_green:
		    glUniform4f( glGetUniformLocation( color_program_, "channel_mask"), 0.0f, c, 0.0f, 1.0f);
		break;

		case image_view_blue:
		    glUniform4f( glGetUniformLocation( color_program_, "channel_mask"), 0.0f, 0.0f, c, 1.0f);
		break;

		default:
		    glUniform4f( glGetUniformLocation( color_program_, "channel_mask"), c, c, c, 1.0f);
	    }
	}

	glColor4f( 0.0, 0.0, 0.0, 1.0);

	glBegin( GL_QUADS);
		glVertex2i( defined_.min.x, defined_.min.y);
		glTexCoord2f( 1, 0);

		glVertex2i( defined_.max.x, defined_.min.y);
		glTexCoord2f( 1, 1);

		glVertex2i( defined_.max.x, defined_.max.y);
		glTexCoord2f( 0, 1);

		glVertex2i( defined_.min.x, defined_.max.y);
		glTexCoord2f( 0, 0);
	glEnd();

	glUseProgram( 0);

	if( checks_ && mode_ == image_view_rgb)
	    glDisable( GL_BLEND);

	glDisable( GL_TEXTURE_2D);
    }

    // draw border around image
    if( texture_id_)
    {
	glColor4f( 0.75, 0.0, 0.0, 1.0);

	glBegin( GL_LINE_LOOP);
	    glVertex2i( domain_.min.x, domain_.min.y);
	    glVertex2i( domain_.max.x, domain_.min.y);
	    glVertex2i( domain_.max.x, domain_.max.y);
	    glVertex2i( domain_.min.x, domain_.max.y);
	glEnd();

	if( overlay_)
	{
	    glColor4f( 0.0, 0.0, 0.75, 1.0);

	    glBegin( GL_LINE_LOOP);
		glVertex2i( defined_.min.x, defined_.min.y);
		glVertex2i( defined_.max.x, defined_.min.y);
		glVertex2i( defined_.max.x, defined_.max.y);
		glVertex2i( defined_.min.x, defined_.max.y);
	    glEnd();
	}
    }
}

void image_view_t::save_projection()
{
    glMatrixMode( GL_PROJECTION);
    glPushMatrix();

    glMatrixMode( GL_MODELVIEW);
    glPushMatrix();
}

void image_view_t::restore_projection()
{
    glMatrixMode( GL_PROJECTION);
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW);
    glPopMatrix();
}

void image_view_t::set_screen_projection()
{
    glMatrixMode( GL_PROJECTION);
    glLoadIdentity();
    glViewport( 0, 0, width(), height());
    gluOrtho2D( 0, width(), height(), 0);
    glMatrixMode( GL_MODELVIEW);
}

void image_view_t::set_ofx_projection()
{
    if( valid_texture_)
    {
	glMatrixMode( GL_PROJECTION);
	float y_off = ( domain_.min.y + ( domain_.size().y / 2)) * subsample();
        glTranslatef( 0, y_off, 0);
        glScalef( 1, -1, 1);
	glTranslatef( 0, -y_off, 0);
        glScalef( subsample(), subsample(), 1);
	glMatrixMode( GL_MODELVIEW);
    }
}

void image_view_t::draw_selection_box( int x0, int y0, int x1, int y1)
{
    glBegin( GL_LINE_LOOP);
	glVertex2i( x0, y0);
	glVertex2i( x1, y0);
	glVertex2i( x1, y1);
	glVertex2i( x0, y1);
    glEnd();
}

void image_view_t::hideEvent( QHideEvent *event)
{
    event->accept();
}

void image_view_t::showEvent( QShowEvent *event)
{
    event->accept();
}

} // namespace
} // namespace
