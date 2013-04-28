// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/flipbook.hpp>

#include<iostream>

#include<QApplication>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QCloseEvent>
#include<QMessageBox>

#include<OpenEXR/half.h>
#include<OpenEXR/ImfInputFile.h>

#include<ramen/GL/glu.hpp>

#include<ramen/ui/image_view_shaders.hpp>
#include<ramen/ui/user_interface.hpp>

#include<ramen/app/preferences.hpp>
#include<ramen/imageio/read_exr.hpp>

namespace ramen
{

namespace flipbook
{

flipbook_t::flipbook_t() : QGLWidget(), frt_(0), playing_(false)
{
    setFocusPolicy( Qt::ClickFocus);
    setMouseTracking( true);
    first_time_ = true;

    exposure_ = 0;
    gamma_ = preferences_t::Instance().display_gamma();

    buffer_index_ = 0;

    timer_.setDesiredFrameRate( preferences_t::Instance().frame_rate());
    framenumber_ = 0;

    tonemap_ = false;
}

void flipbook_t::init( int w, int h)
{
    width_ = w;
    height_ = h;
    
    // initialize frame buffers
    ib_.rgbMode = true;
    ib_.dataWindow = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w - 1, h - 1));

    for (int i = 0; i < ib_.numBuffers(); ++i)
    {
	Imf::FrameBuffer &fb = ib_.frameBuffer( i);

	size_t pixelSize = sizeof( half) * 4;
	size_t lineSize = pixelSize * w;

	char *pixels = new char[lineSize * h];
	pixels_.push_back( pixels);

	ib_.pixels( i, 0) = pixels;
	ib_.pixels( i, 1) = pixels + sizeof( half);
	ib_.pixels( i, 2) = pixels + sizeof( half) * 2;

	fb.insert( "R", Imf::Slice( Imf::HALF, ib_.pixels( i, 0), pixelSize, lineSize, 1, 1));
	fb.insert( "G", Imf::Slice( Imf::HALF, ib_.pixels( i, 1), pixelSize, lineSize, 1, 1));
	fb.insert( "B", Imf::Slice( Imf::HALF, ib_.pixels( i, 2), pixelSize, lineSize, 1, 1));
    }

    // initialize texture
    makeCurrent();
    texture_id_ = 0;

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glGenTextures( 1, &texture_id_);

    glBindTexture( GL_TEXTURE_2D, texture_id_);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, w, h, 0, GL_RGBA, GL_HALF_FLOAT_ARB, 0);

    // check opengl errors here

    resize( w, h);
}

void flipbook_t::free_all_memory()
{
    makeCurrent();

    if( texture_id_)
	glDeleteTextures( 1, &texture_id_);

    for( int i = 0; i < pixels_.size(); ++i)
	delete[] pixels_[i];

    delete frt_;

    frames_.clear();
}

void flipbook_t::set_frame_rate( float fps) { timer_.setDesiredFrameRate( fps);}

void flipbook_t::add_frame( std::auto_ptr<imageio::imf_memory_ostream> f)
{
    assert( f.get());

    if( frames_.empty())
    {
	imageio::imf_memory_istream is( f->data(), f->size());
	Imf::InputFile ifile( is);
	Imath::Box2i dw = ifile.header().dataWindow();
	init( dw.max.x - dw.min.x + 1, dw.max.y - dw.min.y + 1);
    }

    frames_.push_back( f);

    show();
    load_texture( frames_.back());
    update();
    
    qApp->processEvents();
}

imageio::imf_memory_istream flipbook_t::stream_for_frame( int f)
{
    assert( f >= 0);
    assert( f < frames_.size());
    return imageio::imf_memory_istream( frames_[f].data(), frames_[f].size());
}

void flipbook_t::play()
{
    std::size_t size = 0;
    for( int i = 0; i < num_frames(); ++i)
	size += frames_[i].size();

    playing_ = true;
    update();
}

void flipbook_t::initializeGL()
{
    glClearColor( .3, .3, .3, 0);

    if( first_time_)
    {
	test_gl_extensions();
	viewport_.reset( width(), height());
	first_time_ = false;
    }

    color_shader_ = glCreateShader( GL_FRAGMENT_SHADER);
    glShaderSource( color_shader_, 1, &ui::shaders::color_shader_source, 0);
    glCompileShader( color_shader_);
    color_program_ = glCreateProgram();
    glAttachShader( color_program_, color_shader_);
    glLinkProgram( color_program_);
    glUseProgram( color_program_);

    glUseProgram(0);
}

void flipbook_t::resizeGL( int w, int h) { viewport_.resize( w, h);}

void flipbook_t::paintGL()
{
    if( !playing_)
    {
	draw_frame();
	glFlush();
	return;
    }

    if( !frt_)
    {
	frt_ = new FileReadingThread( this);
	timer_.playState = ( num_frames() != 1) ? RUNNING: PREPARE_TO_PAUSE;
    }

    if( ib_.exitSemaphore2.tryWait())
	close();

    //
    // Wait until it is time to display the next image
    //

    timer_.waitUntilNextFrameIsDue();

    //
    // Wait until the file reading thread has made the next frame available
    //

    if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_PAUSE)
	ib_.fullBuffersSemaphore.wait();

    if( timer_.playState == PREPARE_TO_PAUSE)
	timer_.playState = PAUSE;

    framenumber_ = ib_.frameNumber( buffer_index_);
    update_texture();
    draw_frame();

    //
    // Return the image buffer to the file reading thread
    //
    if (timer_.playState == RUNNING || timer_.playState == PREPARE_TO_RUN)
    {
	buffer_index_ = (buffer_index_ + 1) % ib_.numBuffers();
	ib_.emptyBuffersSemaphore.post();
    }

    if (timer_.playState == PREPARE_TO_RUN)
	timer_.playState = RUNNING;

    glFlush();

    // Make sure this function gets called again immediately
    if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_RUN)
	update();
}

void flipbook_t::closeEvent( QCloseEvent *event)
{
    if( playing_)
    {
	ib_.exitSemaphore1.post();
	ib_.emptyBuffersSemaphore.post();
	ib_.exitSemaphore2.wait();
    }

    hide();
    closed( this);

    free_all_memory();
    deleteLater();
    event->accept();
}

void flipbook_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Escape:
	close();
	event->accept();
    break;

    case Qt::Key_T:
	tonemap_ = !tonemap_;
	update();
	event->accept();
    break;

    case Qt::Key_Plus:	// exposure +
	exposure_ += 1.0f;
	update();
	event->accept();
    break;

    case Qt::Key_Minus:	// exposure -
	exposure_ -= 1.0f;
	update();
	event->accept();
    break;

    case Qt::Key_0:	// exposure 0
	exposure_ = 0;
	update();
	event->accept();
    break;
    }

    if( playing_)
    {
	switch( event->key())
	{
	case Qt::Key_P: // pause, loop forward
	case Qt::Key_L:
	{
	    if ( timer_.playState == RUNNING && ib_.forward)
		timer_.playState = PREPARE_TO_PAUSE;

	    if( timer_.playState == PAUSE)
		timer_.playState = PREPARE_TO_RUN;

	    ib_.forward = true;
	    update();
	    event->accept();
	}
	break;

	case Qt::Key_H: // pause, loop backward
	{
	    if( timer_.playState == RUNNING && !ib_.forward)
		timer_.playState = PREPARE_TO_PAUSE;

	    if( timer_.playState == PAUSE)
		timer_.playState = PREPARE_TO_RUN;

	    ib_.forward = false;
	    update();
	    event->accept();
	}
	break;

	case Qt::Key_J:
	case Qt::Key_K:
	{
	    // Step one frame forward or backward
	    if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_PAUSE)
		ib_.fullBuffersSemaphore.wait();

	    if( event->key() == Qt::Key_K)
		ib_.forward = true;
	    else
		ib_.forward = false;

	    timer_.playState = PAUSE;

	    int new_framenumber;

	    if( ib_.forward)
	    {
		if( framenumber_ >= num_frames() - 1)
		    new_framenumber = 0;
		else
		    new_framenumber = framenumber_ + 1;
	    }
	    else
	    {
		if( ib_.frameNumber( buffer_index_) <= 0)
		    new_framenumber = num_frames() - 1;
		else
		    new_framenumber = framenumber_ - 1;
	    }

	    while( ib_.frameNumber( buffer_index_) != new_framenumber)
	    {
		buffer_index_ = ( buffer_index_ + 1) % ib_.numBuffers();
		ib_.emptyBuffersSemaphore.post();
		ib_.fullBuffersSemaphore.wait();
	    }

	    update();
	    event->accept();
	}
	break;

	case Qt::Key_F:
	    // handle fullscreen
	    event->accept();
	break;

	default:
	    event->ignore();
	}
    }
}

void flipbook_t::keyReleaseEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Escape:
    case Qt::Key_T:
    case Qt::Key_Plus:	// exposure +
    case Qt::Key_Minus:	// exposure -
    case Qt::Key_0:	// exposure 0
	event->accept();
	return;
    break;
    }

    if( playing_)
    {
	switch( event->key())
	{
	case Qt::Key_P:
	case Qt::Key_L:
	case Qt::Key_H:
	case Qt::Key_J:
	case Qt::Key_K:
	case Qt::Key_F:
	    event->accept();
	break;

	default:
	    event->ignore();
	}
    }
    else
	event->ignore();
}

void flipbook_t::fatal_gl_error( const QString& message) const
{
    QMessageBox::warning( (QWidget *) ui::user_interface_t::Instance().main_window(),
			 "OpenGL error", message);
    exit( 0);
}

void flipbook_t::test_gl_extensions()
{
    GLenum err = glewInit();

    if( GLEW_OK != err)
	fatal_gl_error( "Video card not supported: Error initializing GLEW");

    if( !glewIsSupported("GL_VERSION_2_0"))
        fatal_gl_error( "Video card not supported: Not supported OpenGL 2.0");

    if( !GLEW_ARB_texture_non_power_of_two)
        fatal_gl_error( "Video card not supported: Not supported non power of two textures");

    if( !GLEW_ARB_texture_float)
        fatal_gl_error( "Video card not supported: Not supported float textures");

    if( !GLEW_ARB_fragment_shader)
        fatal_gl_error( "Video card not supported: Not supported fragment programs");

    // Todo: Test Half textures here
}

void flipbook_t::draw_frame()
{
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode( GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode( GL_PROJECTION);
    glLoadIdentity();
    glViewport( 0, 0, width(), height());
    gluOrtho2D( viewport_.world().min.x, viewport_.world().max.x, viewport_.world().max.y, viewport_.world().min.y);
    glMatrixMode( GL_MODELVIEW);

    float img_zoom_ = 1.0f;
    
    glScalef( img_zoom_, img_zoom_, 1);

    glClear( GL_COLOR_BUFFER_BIT);

    if( texture_id_)
    {
	glEnable( GL_TEXTURE_2D);

	glBindTexture( GL_TEXTURE_2D, texture_id_);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glUseProgram( color_program_);
	glUniform1i( glGetUniformLocation( color_program_, "tex"), 0);
	glUniform1f( glGetUniformLocation( color_program_, "gamma"), 1.0f / gamma_);

	float c = 1.0f;

	if( tonemap_)
	{
	    glUniform1f( glGetUniformLocation( color_program_, "exposure"), std::pow( 2.0f, exposure_ + 2.47393f));
	    c = 84.66f / 255.0f;
	}
	else
	    glUniform1f( glGetUniformLocation( color_program_, "exposure"), 1.0f);

	glUniform4f( glGetUniformLocation( color_program_, "channel_mask"), c, c, c, 1.0f);

	float img_x_offset_ = 0;
	float img_y_offset_ = 0;

	glColor4f( 0.0, 0.0, 0.0, 1.0);

	glBegin( GL_QUADS);
	    glVertex2i( img_x_offset_, img_y_offset_);
	    glTexCoord2f( 1, 0);

	    glVertex2i( width_ + img_x_offset_, img_y_offset_);
	    glTexCoord2f( 1, 1);

	    glVertex2i( width_ + img_x_offset_, height_ + img_y_offset_);
	    glTexCoord2f( 0, 1);

	    glVertex2i( img_x_offset_, height_ + img_y_offset_);
	    glTexCoord2f( 0, 0);
	glEnd();

	glUseProgram( 0);
	glDisable( GL_TEXTURE_2D);
    }
}

void flipbook_t::load_texture( const imageio::imf_memory_ostream& os)
{
    makeCurrent();
    
    boost::gil::rgba16f_image_t image;
    imageio::imf_memory_istream is( os.data(), os.size());
    imageio::read_exr_image( is, image);

    boost::gil::rgba16f_view_t view( boost::gil::view( image));

    // load in texture
    glEnable( GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture_id_);
    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width_, height_, 0, GL_RGBA, GL_HALF_FLOAT_ARB, ptr);
    glDisable( GL_TEXTURE_2D);
}

void flipbook_t::update_texture()
{
    glEnable( GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture_id_);
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, GL_HALF_FLOAT_ARB, (GLvoid *) ib_.pixels( buffer_index_, 0));
    glDisable( GL_TEXTURE_2D);
}

} // namespace
} // namespace
