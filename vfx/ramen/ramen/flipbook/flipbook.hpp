// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_FLIPBOOK_HPP
#define RAMEN_FLIPBOOK_FLIPBOOK_HPP

#include<ramen/flipbook/flipbook_fwd.hpp>

#include<GL/glew.h>

#include<QGLWidget>

#include<memory.h>

#include<boost/signal.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/ui/viewport.hpp>

#include<ramen/flipbook/Timer.h>
#include<ramen/flipbook/ImageBuffers.h>
#include<ramen/flipbook/FileReadingThread.h>
#include<ramen/imageio/imf_memory_stream.hpp>

namespace ramen
{
namespace flipbook
{

class flipbook_t : public QGLWidget
{
    Q_OBJECT

public:

    flipbook_t();
    virtual ~flipbook_t() {}

    void init( int w, int h);
    void free_all_memory();

    boost::signal<void (flipbook_t *)> closed;

    void set_frame_rate( float fps);

    bool empty() const              { return frames_.empty();}
    std::size_t num_frames() const  { return frames_.size();}

    void add_frame( std::auto_ptr<imageio::imf_memory_ostream> f);
    imageio::imf_memory_istream stream_for_frame( int f);

    ImageBuffers& image_buffers() { return ib_;}

    void play();

protected:

    virtual void initializeGL();
    virtual void resizeGL( int w, int h);
    virtual void paintGL();

    virtual void closeEvent( QCloseEvent *event);

    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);

private:

    void fatal_gl_error( const QString& message) const;

    void test_gl_extensions();

    void draw_frame();

    void load_texture( const imageio::imf_memory_ostream& os);
    void update_texture();

    bool first_time_;
    ui::viewport_t viewport_;

    int width_, height_;

    bool playing_;

    FileReadingThread *frt_;
    ImageBuffers ib_;
    Timer timer_;
    int framenumber_;

    float exposure_, gamma_;

    boost::ptr_vector<imageio::imf_memory_ostream> frames_;

    std::vector<char *> pixels_;
    int buffer_index_;

    GLuint texture_id_;
    GLuint color_program_, color_shader_, color_sampler_param_;

    bool tonemap_;
};

} // namespace
} // namespace

#endif
