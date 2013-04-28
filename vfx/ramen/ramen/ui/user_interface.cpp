// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/user_interface.hpp>

#include<iostream>

#include<boost/foreach.hpp>
#include<boost/filesystem/operations.hpp>

#include<QApplication>
#include<QSplashScreen>
#include<QMessageBox>
#include<QFile>
#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>
#include<ramen/undo/undo_stack.hpp>
#include<ramen/image/image_cache.hpp>
#include<ramen/imageio/factory.hpp>

#include<ramen/ui/main_window.hpp>
#include<ramen/ui/time_slider.hpp>
#include<ramen/ui/inspector.hpp>
#include<ramen/ui/viewer.hpp>
#include<ramen/ui/render_dialog.hpp>
#include<ramen/ui/render_flipbook_dialog.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

#include<ramen/system/system.hpp>

#include<ramen/util/path.hpp>

namespace ramen
{
namespace ui
{

user_interface_impl::user_interface_impl() : QObject(), active_(0), context_(0), quitting_(false) {}

user_interface_impl::~user_interface_impl() {}

void user_interface_impl::init()
{
    #ifdef __APPLE__
    QFont default_font( qApp->font());
    default_font.setPointSize( default_font.pointSize() - 2);
    qApp->setFont( default_font);
    #endif

    image_types_str_ = "Image Files (";

    BOOST_FOREACH( const std::string& ext, imageio::factory_t::Instance().extensions())
    {
	image_types_str_.append( "*.");
	image_types_str_.append( ext.c_str());
	image_types_str_.append( " ");
    }

    image_types_str_.append( ")");
    
    window_ = new main_window_t();

    // force creation of singletons to avoid a crash at exit
    viewer_t::Instance();
    inspector_t::Instance();
    anim_editor_t::Instance();

    create_new_document();

    // restore the last saved window state
    boost::filesystem::path p = system::system_t::Instance().preferences_path() / "wstate.ui";

    QFile file( p.external_file_string().c_str());

    if( file.open( QIODevice::ReadOnly))
    {
	QByteArray window_state = file.readAll();
	window_->restoreState( window_state);
    }
}

void user_interface_impl::show() { window_->show();}

int user_interface_impl::run()
{
    try
    {
	return qApp->exec();
    }
    catch( std::exception& e)
    {
        application_t::Instance().error( std::string( "user_interface_t::run, exception thrown. what = ") + e.what());
    }
    catch( ...)
    {
        application_t::Instance().error( "user_interface_t::run, unknown exception thrown.");
    }

    return 0;
}

void user_interface_impl::quit()
{
    undo::undo_stack_t::Instance().clear();
    Loki::DeletableSingleton<document_impl>::GracefulDelete();

    quitting_ = true;

    QByteArray window_state = window_->saveState();
    boost::filesystem::path p = system::system_t::Instance().preferences_path() / "wstate.ui";
    QFile file( p.external_file_string().c_str());

    if( file.open( QIODevice::WriteOnly))
	file.write( window_state);

    qApp->quit();
}

void user_interface_impl::create_new_document()
{
    set_active_node( 0);
    set_context_node( 0);
    undo::undo_stack_t::Instance().clear();
    Loki::DeletableSingleton<document_impl>::GracefulDelete();
    document_t::Instance().attach_release_observer( boost::bind( &user_interface_impl::node_released, this, _1));

    render_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						document_t::Instance().composition().end_frame());

    render_dialog_t::Instance().set_mblur_settings( 0, 1);

    render_flipbook_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						document_t::Instance().composition().end_frame());
    
    image::image_cache_t::Instance().clear();
    update();
}

void user_interface_impl::open_document( const boost::filesystem::path& p, bool no_gui)
{
    if( no_gui)
    {
	try
	{
	    document_t::Instance().load( p);
	    render_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						    document_t::Instance().composition().end_frame());

            render_dialog_t::Instance().set_mblur_settings( 0, 1);

            render_flipbook_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						    document_t::Instance().composition().end_frame());
	}
	catch( boost::archive::archive_exception& e)
	{
            application_t::Instance().error( std::string( "user_interface_t::open document, exception thrown. what = ") + e.what());
	}
    }
    else
    {
	if( main_window()->can_close_document())
	{
	    user_interface_t::Instance().create_new_document();
	    main_window()->open_document( p);
	}
    }
}

void user_interface_impl::set_active_node( node_t *n)
{
    if( active_ != n)
    {
	if( active_)
	    active_->end_edit();

        active_ = n;

	if( active_)
	    active_->begin_edit();

        inspector_t::Instance().edit_node( n);
        viewer_t::Instance().set_active_node( n);
	anim_editor_t::Instance().set_active_node( n);
    }
}

void user_interface_impl::set_context_node( node_t *n)
{
    if( context_ != n)
    {
	if( context_)
	    context_->end_context();

	context_ = n;

	if( context_)
	    context_->begin_context();

	viewer_t::Instance().set_context_node( n);
    }
}

void user_interface_impl::node_released( node_t *n)
{
    if( n == active_)
    {
	set_active_node( 0);
	update();
    }

    if( n == context_)
    {
	set_context_node( 0);
	update();
    }

    inspector_t::Instance().delete_panel( n);
    viewer_t::Instance().node_released( n);
    anim_editor_t::Instance().node_released( n);
}

void user_interface_impl::update()
{
    if( !quitting_)
        window_->update();
}

int user_interface_impl::start_frame() const
{
    return document_t::Instance().composition().start_frame();
}

int user_interface_impl::end_frame() const
{
    return document_t::Instance().composition().end_frame();
}

int user_interface_impl::frame() const
{
    return document_t::Instance().composition().frame();
}

void user_interface_impl::set_start_frame( int t)
{
    document_t::Instance().composition().set_start_frame( t);
    main_window()->time_slider()->update( document_t::Instance().composition().start_frame(),
					 document_t::Instance().composition().frame(),
					 document_t::Instance().composition().end_frame());

    render_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						    document_t::Instance().composition().end_frame());

    render_flipbook_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
		    				document_t::Instance().composition().end_frame());
}

void user_interface_impl::set_end_frame( int t)
{
    document_t::Instance().composition().set_end_frame( t);
    main_window()->time_slider()->update( document_t::Instance().composition().start_frame(), 
					 document_t::Instance().composition().frame(), 
					 document_t::Instance().composition().end_frame());

    render_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						    document_t::Instance().composition().end_frame());

    render_flipbook_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
		    				document_t::Instance().composition().end_frame());
}

void user_interface_impl::set_frame( int t)
{
    document_t::Instance().composition().set_frame( t);
    main_window()->time_slider()->update( document_t::Instance().composition().start_frame(),
					 document_t::Instance().composition().frame(),
					 document_t::Instance().composition().end_frame());

    inspector_t::Instance().update();
    anim_editor_t::Instance().update();
    viewer_t::Instance().frame_changed();
}

void user_interface_impl::fatal_error( const std::string& msg)
{
    QMessageBox::critical( 0, "Fatal Error", msg.c_str());

    // try to save the composition.
    // at this point, we have nothing to loose
    // and much to win
    main_window()->save_document_as();
}

void user_interface_impl::error( const std::string& msg)
{
    QMessageBox::warning( main_window(), "Error", msg.c_str());
}

void user_interface_impl::inform( const std::string& msg)
{
    QMessageBox::information( main_window(), "Info", msg.c_str());
}

bool user_interface_impl::question( const std::string& what, bool default_answer)
{
    QMessageBox::StandardButton result;

    if( default_answer)
    {
	result = QMessageBox::question( main_window(), "Question", what.c_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	return result == QMessageBox::Yes;
    }
    else
    {
	result = QMessageBox::question( main_window(), "Question", what.c_str(), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	return result == QMessageBox::Yes;
    }
}

bool user_interface_impl::image_sequence_file_selector( boost::filesystem::path& p, bool& sequence, bool& relative) const
{
    std::string types( image_types_string().toStdString());
    return image_sequence_file_selector( "Open Image", types, p, sequence, relative);
}

bool user_interface_impl::image_sequence_file_selector( const std::string& title, const std::string& types, 
						       boost::filesystem::path& p, bool& sequence, bool& relative) const
{
    static bool was_relative = false;
    static bool was_sequence = true;

    QFileDialog dialog( 0, QObject::tr( title.c_str()), QString::null, types.c_str());

    #ifdef __APPLE__
	dialog.setOption( QFileDialog::DontUseNativeDialog);
    #endif

    dialog.setFileMode( QFileDialog::ExistingFile);

    QWidget *extra_widgets = new QWidget( &dialog);
    QHBoxLayout *layout = new QHBoxLayout( extra_widgets);

    QCheckBox *relative_check = new QCheckBox( QObject::tr( "Relative"));

    if( document_t::Instance().has_file())
	relative_check->setChecked( was_relative);
    else
    {
	was_relative = false;
	relative_check->setChecked( false);
	relative_check->setEnabled( false);
    }

    QCheckBox *sequence_check = new QCheckBox( QObject::tr( "Sequence"));
    sequence_check->setChecked( was_sequence);

    layout->addWidget( relative_check);
    layout->addWidget( sequence_check);

    QGridLayout *glayout = (QGridLayout *) dialog.layout();
    glayout->addWidget( extra_widgets, glayout->rowCount(), 0, 1, glayout->columnCount(), Qt::AlignLeft);

    dialog.show();

    QStringList filenames;

    if( dialog.exec())
    {
	filenames = dialog.selectedFiles();
	p = filenames[0].toStdString();

	relative = relative_check->isChecked();
	was_relative = relative;

	if( relative_check->isChecked())
	{
	    boost::filesystem::path dir = document_t::Instance().file().parent_path();
	    p = make_relative_path( p, dir);
	}

	sequence = sequence_check->isChecked();
	was_sequence = sequence;
	return true;
    }

    return false;
}

} // namespace
} // namespace
