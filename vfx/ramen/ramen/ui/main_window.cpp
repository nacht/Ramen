// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/main_window.hpp>

#include<ramen/config.hpp>

#include<iostream>
#include<map>

#include<boost/foreach.hpp>

#include<QApplication>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QMessageBox>
#include<QVBoxLayout>
#include<QCloseEvent>
#include<QFileDialog>
#include<QProgressDialog>
#include<QDesktopServices>
#include<QDockWidget>
#include<QDesktopWidget>
#include<QUrl>
#include<QStatusBar>
#include<QSettings>
#include<QStringList>
#include<QFileInfo>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector.hpp>
#include<ramen/ui/viewer.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/render_dialog.hpp>
#include<ramen/ui/render_flipbook_dialog.hpp>
#include<ramen/ui/add_node_command.hpp>
#include<ramen/ui/edit_commands.hpp>
#include<ramen/ui/composition_settings_dialog.hpp>
#include<ramen/ui/preferences_dialog.hpp>
#include<ramen/ui/composition_view.hpp>
#include<ramen/ui/time_slider.hpp>
#include<ramen/ui/vcr_controls.hpp>
#include<ramen/ui/python/python_console.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/export_selected_nodes.hpp>
#include<ramen/app/import_composition.hpp>

#include<ramen/nodes/node_factory.hpp>
#include<ramen/nodes/base/image_output_node.hpp>

#include<ramen/undo/undo_stack.hpp>

#include<ramen/render/render_sequence.hpp>
#include<ramen/render/render_flipbook.hpp>

#include<ramen/image/image_cache.hpp>
#include<ramen/imageio/import_multichannel_exr.hpp>

#include<ramen/flipbook/flipbook.hpp>
#include<ramen/system/system.hpp>
#include<ramen/ofx/plugin_manager.hpp>

namespace ramen
{
namespace ui
{

main_window_t::main_window_t() : QMainWindow()
{
    #ifdef __APPLE__
	menubar_ = new QMenuBar();
    #else
	menubar_ = menuBar();
    #endif

    create_actions();
    create_menus();

   // docks
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Inspector
    inspector_dock_ = new QDockWidget( "Inspector", this);
    inspector_dock_->setObjectName( "inspector_dock");
    inspector_dock_->setAllowedAreas( Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);
    inspector_dock_->setWidget( inspector_t::Instance().widget());
    addDockWidget( Qt::RightDockWidgetArea, inspector_dock_);
    view_->addAction( inspector_dock_->toggleViewAction());

    // vcr controls
    vcr_controls_dock_ = new QDockWidget( "VCR Controls", this);
    vcr_controls_dock_->setObjectName( "vcr_controls_dock");
    vcr_controls_dock_->setAllowedAreas( Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);
    vcr_controls_dock_->setWidget( vcr_controls_t::Instance().widget());
    addDockWidget( Qt::RightDockWidgetArea, vcr_controls_dock_);
    view_->addAction( vcr_controls_dock_->toggleViewAction());

    // anim editor dock
    anim_editor_dock_ = new QDockWidget( "Anim Editor", this);
    anim_editor_dock_->setObjectName( "anim_editor_dock");
    anim_editor_dock_->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea |
					Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    anim_editor_dock_->setWidget( anim_editor_t::Instance().widget());
    addDockWidget( Qt::BottomDockWidgetArea, anim_editor_dock_);
    view_->addAction( anim_editor_dock_->toggleViewAction());

    // Composition view
    composition_dock_ = new QDockWidget( "Composition", this);
    composition_dock_->setObjectName( "composition_dock");
    composition_dock_->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea |
					Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    comp_view_ = new composition_view_t();
    composition_dock_->setWidget( comp_view_);
    addDockWidget( Qt::BottomDockWidgetArea, composition_dock_);
    view_->addAction( composition_dock_->toggleViewAction());

    tabifyDockWidget( anim_editor_dock_, composition_dock_);

    // python console
    py_console_dock_ = new QDockWidget( "Python Console", this);
    py_console_dock_->setObjectName( "py_console_dock");
    py_console_dock_->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea |
					Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    py_console_dock_->setWidget( python_console_t::Instance().widget());
    addDockWidget( Qt::BottomDockWidgetArea, py_console_dock_);
    view_->addAction( py_console_dock_->toggleViewAction());

    tabifyDockWidget( anim_editor_dock_, py_console_dock_);

    // image view
    QWidget *viewer_widget = new QWidget();
    time_slider_ = new time_slider_t();
    connect( time_slider_, SIGNAL( start_frame_changed( int)), &user_interface_t::Instance(), SLOT( set_start_frame( int)));
    connect( time_slider_, SIGNAL( end_frame_changed( int)), &user_interface_t::Instance(), SLOT( set_end_frame( int)));
    connect( time_slider_, SIGNAL( time_changed( int)), &user_interface_t::Instance(), SLOT( set_frame( int)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing( 0);
    layout->setContentsMargins( 0, 0, 0, 0);
    layout->addWidget( viewer_t::Instance().widget());
    layout->addWidget( time_slider_);
    viewer_widget->setLayout( layout);
    setCentralWidget( viewer_widget);

    // add hotkeys actions here

    // create the status bar
    statusBar()->showMessage( RAMEN_NAME_VERSION_STR);

    QRect screen_size = qApp->desktop()->availableGeometry();
    move( screen_size.left(), screen_size.top());
    resize( screen_size.width(), screen_size.height() - 40);
    setWindowTitle( "Ramen");

    setWindowIcon( QIcon( ":small_app_icon.png"));
}

void main_window_t::closeEvent( QCloseEvent *event)
{
    quit();

    if( user_interface_t::Instance().quitting())
	event->accept();
    else
	event->ignore();
}

void main_window_t::create_actions()
{
    new_ = new QAction( tr( "New"), this);
    new_->setShortcut( tr( "Ctrl+N"));
    new_->setShortcutContext( Qt::ApplicationShortcut);
    connect( new_, SIGNAL( triggered()), this, SLOT( new_document()));

    open_ = new QAction( tr( "Open..."), this);
    open_->setShortcut( tr( "Ctrl+O"));
    open_->setShortcutContext( Qt::ApplicationShortcut);
    connect( open_, SIGNAL( triggered()), this, SLOT( open_document()));

    for( int i = 0; i < max_recently_opened_files; ++i)
    {
	recently_opened_[i] = new QAction( this);
	recently_opened_[i]->setVisible( false);
	connect( recently_opened_[i], SIGNAL( triggered()), this, SLOT( open_recent_document()));
    }

    save_ = new QAction( tr( "Save"), this);
    save_->setShortcut( tr( "Ctrl+S"));
    save_->setShortcutContext( Qt::ApplicationShortcut);
    connect( save_, SIGNAL( triggered()), this, SLOT( save_document()));

    save_as_ = new QAction( tr( "Save As..."), this);
    save_as_->setShortcutContext( Qt::ApplicationShortcut);
    connect( save_as_, SIGNAL( triggered()), this, SLOT( save_document_as()));

    quit_ = new QAction( tr( "Quit"), this);
    quit_->setShortcut( tr( "Ctrl+Q"));
    quit_->setShortcutContext( Qt::ApplicationShortcut);
    connect( quit_, SIGNAL( triggered()), this, SLOT( quit()));

    import_comp_ = new QAction( tr( "Import Composition..."), this);
    connect( import_comp_, SIGNAL( triggered()), this, SLOT( import_composition()));

    export_sel_ = new QAction( tr( "Export Selection..."), this);
    connect( export_sel_, SIGNAL( triggered()), this, SLOT( export_selection()));

    import_multiexr_ = new QAction( tr( "Import All EXR Layers..."), this);
    connect( import_multiexr_, SIGNAL( triggered()), this, SLOT( import_multilayer_exr()));

    undo_ = new QAction( tr( "Undo"), this);
    undo_->setShortcut( tr( "Ctrl+Z"));
    undo_->setShortcutContext( Qt::ApplicationShortcut);
    connect( undo_, SIGNAL( triggered()), this, SLOT( undo()));

    redo_ = new QAction( tr( "Redo"), this);
    redo_->setShortcut( tr( "Ctrl+Shift+Z"));
    redo_->setShortcutContext( Qt::ApplicationShortcut);
    connect( redo_, SIGNAL( triggered()), this, SLOT( redo()));

    ignore_ = new QAction( tr( "Ignore"), this);
    ignore_->setShortcut( tr( "Ctrl+I"));
    ignore_->setShortcutContext( Qt::ApplicationShortcut);
    connect( ignore_, SIGNAL( triggered()), this, SLOT( ignore_nodes()));

    delete_ = new QAction( tr( "Delete"), this);
    connect( delete_, SIGNAL( triggered()), this, SLOT( delete_nodes()));

    duplicate_ = new QAction( tr( "Duplicate"), this);
    duplicate_->setShortcut( tr( "Ctrl+D"));
    duplicate_->setShortcutContext( Qt::ApplicationShortcut);
    connect( duplicate_, SIGNAL( triggered()), this, SLOT( duplicate_nodes()));

    clear_cache_ = new QAction( tr( "Clear Image Cache"), this);
    connect( clear_cache_, SIGNAL( triggered()), this, SLOT( clear_cache()));

    preferences_ = new QAction( tr( "Preferences..."), this);
    connect( preferences_, SIGNAL( triggered()), this, SLOT( show_preferences_dialog()));

    comp_settings_ = new QAction( tr( "Composition Settings..."), this);
    comp_settings_->setShortcut( tr( "Ctrl+K"));
    comp_settings_->setShortcutContext( Qt::ApplicationShortcut);
    connect( comp_settings_, SIGNAL( triggered()), this, SLOT( show_composition_settings_dialog()));

    comp_flipbook_ = new QAction( tr( "Render Flipbook..."), this);
    connect( comp_flipbook_, SIGNAL( triggered()), this, SLOT( render_flipbook()));

    comp_render_ = new QAction( tr( "Render Composition..."), this);
    comp_render_->setShortcut( tr( "Ctrl+R"));
    comp_render_->setShortcutContext( Qt::ApplicationShortcut);
    connect( comp_render_, SIGNAL( triggered()), this, SLOT( render_composition()));

    about_ = new QAction( tr( "About Ramen..."), this);
    connect( about_, SIGNAL( triggered()), this, SLOT( show_about_box()));

    project_web_ = new QAction( tr( "Project Website..."), this);
    connect( project_web_, SIGNAL( triggered()), this, SLOT( go_to_project_website()));

    // non-menu actions
    next_frame_ = new QAction( this);
    next_frame_->setShortcut( Qt::Key_Right);
    next_frame_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
    connect( next_frame_, SIGNAL( triggered()), &(vcr_controls_t::Instance()), SLOT( next_frame()));
    addAction( next_frame_);

    prev_frame_ = new QAction( this);
    prev_frame_->setShortcut( Qt::Key_Left);
    prev_frame_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
    connect( prev_frame_, SIGNAL( triggered()), &(vcr_controls_t::Instance()), SLOT( prev_frame()));
    addAction( prev_frame_);
}

void main_window_t::create_menus()
{
    file_ = menubar_->addMenu( tr( "File"));
	file_->addAction( new_);
	file_->addAction( open_);

	open_recent_ = file_->addMenu( tr( "Open Recent"));
	    for( int i = 0; i < max_recently_opened_files; ++i)
		open_recent_->addAction( recently_opened_[i]);

	init_recent_files_menu();

	file_->addAction( save_);
	file_->addAction( save_as_);
	file_->addSeparator();

	import_ = file_->addMenu( tr( "Import"));
	    import_->addAction( import_comp_);
	    import_->addSeparator();
	    import_->addAction( import_multiexr_);

	export_ = file_->addMenu( tr( "Export"));
	    export_->addAction( export_sel_);

	file_->addSeparator();
	file_->addAction( quit_);

    edit_ = menubar_->addMenu( tr( "Edit"));
        edit_->addAction( undo_);
	edit_->addAction( redo_);
	edit_->addSeparator();
	edit_->addAction( ignore_);
	edit_->addAction( delete_);
	edit_->addAction( duplicate_);
	edit_->addSeparator();
	edit_->addAction( clear_cache_);
	edit_->addSeparator();
	edit_->addAction( preferences_);

    composition_ = menubar_->addMenu( tr( "Composition"));
	composition_->addAction( comp_settings_);
	composition_->addSeparator();
	composition_->addAction( comp_flipbook_);
	composition_->addAction( comp_render_);

    create_ = menubar_->addMenu( tr( "Create"));
	create_submenus_.push_back( create_->addMenu( "Input"));
	create_submenus_.push_back( create_->addMenu( "Channel"));
	create_submenus_.push_back( create_->addMenu( "Color"));
	create_submenus_.push_back( create_->addMenu( "Key"));
	create_submenus_.push_back( create_->addMenu( "Matte"));
	create_submenus_.push_back( create_->addMenu( "Filter"));
	create_submenus_.push_back( create_->addMenu( "Noise"));
	create_submenus_.push_back( create_->addMenu( "Distort"));
	create_submenus_.push_back( create_->addMenu( "Inpaint"));
	create_submenus_.push_back( create_->addMenu( "Transform"));
	create_submenus_.push_back( create_->addMenu( "Track"));
	create_submenus_.push_back( create_->addMenu( "Layer"));
	create_submenus_.push_back( create_->addMenu( "Render"));
	create_submenus_.push_back( create_->addMenu( "Lighting"));
	create_submenus_.push_back( create_->addMenu( "Tone Map"));
	create_submenus_.push_back( create_->addMenu( "Time"));
	create_submenus_.push_back( create_->addMenu( "Util"));
	create_submenus_.push_back( create_->addMenu( "Output"));

	create_ofx_ = menubar_->addMenu( "OFX");

    create_node_actions();

    // TODO: review this
    for( unsigned int i = 0; i < create_submenus_.size(); ++i)
        connect( create_submenus_[i], SIGNAL( triggered( QAction*)), this, SLOT( create_node( QAction*)));

    for( unsigned int i = 0; i < ofx_submenus_.size(); ++i)
	connect( ofx_submenus_[i], SIGNAL( triggered( QAction*)), this, SLOT( create_node( QAction*)));

    view_ = menubar_->addMenu( tr( "Window"));

    help_ = menubar_->addMenu( tr( "Help"));
	help_->addAction( about_);
	help_->addAction( project_web_);

}

std::vector<QMenu*>::iterator main_window_t::find_submenu( const std::string& name)
{
    for( std::vector<QMenu*>::iterator it( create_submenus_.begin()); it != create_submenus_.end(); ++it)
    {
	if( (*it)->title().toStdString() == name)
	    return it;
    }

    return create_submenus_.end();
}

std::vector<QMenu*>::iterator main_window_t::find_ofx_submenu( const std::string& name)
{
    for( std::vector<QMenu*>::iterator it( ofx_submenus_.begin()); it != ofx_submenus_.end(); ++it)
    {
	if( (*it)->title().toStdString() == name)
	    return it;
    }

    return ofx_submenus_.end();
}

void main_window_t::create_node_actions()
{
    for( node_factory_impl::const_iterator it( node_factory_t::Instance().begin()); it != node_factory_t::Instance().end(); ++it)
    {
	if( it->second.visible)
	{
	    std::vector<QMenu*>::iterator menu_it = find_submenu( it->second.submenu);

	    if( menu_it == create_submenus_.end())
	    {
		create_submenus_.push_back( create_->addMenu( QString( it->second.submenu.c_str())));
		menu_it = create_submenus_.end() - 1;
	    }

	    QAction *act = new QAction( QString( it->second.menu_item.c_str()), this);
	    (*menu_it)->addAction( act);
	    create_node_actions_[act] = it->second.id;
	}
    }

    for( ofx::plugin_manager_t::const_iterator it( application_t::Instance().ofx_plugin_manager().begin());
	    it != application_t::Instance().ofx_plugin_manager().end(); ++it)
    {
	std::vector<QMenu*>::iterator menu_it = find_ofx_submenu( it->group);

	if( menu_it == ofx_submenus_.end())
	{
	    ofx_submenus_.push_back( create_ofx_->addMenu( QString( it->group.c_str())));
	    menu_it = ofx_submenus_.end() - 1;
	}

	QAction *act = new QAction( QString( it->label.c_str()), this);
	(*menu_it)->addAction( act);
	create_node_actions_[act] = it->id;
    }
}

bool main_window_t::can_close_document()
{
    if( document_t::Instance().dirty())
    {
        int r = QMessageBox::warning( this, tr( "Ramen"),
                        tr( "The composition has been modified.\n"
                            "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);

        if (r == QMessageBox::Yes)
	{
	    save_document();
            return document_t::Instance().dirty();
	}
        else
	    if( r == QMessageBox::Cancel)
		return false;
    }

    return true;
}

void main_window_t::new_document()
{
    if( can_close_document())
	user_interface_t::Instance().create_new_document();
}

void main_window_t::open_document( const boost::filesystem::path& p)
{
    try
    {
	document_t::Instance().load( p);

	undo::undo_stack_t::Instance().clear();
	render_dialog_t::Instance().set_frame_range( document_t::Instance().composition().start_frame(),
						document_t::Instance().composition().end_frame());

        render_dialog_t::Instance().set_mblur_settings( 0, 1);

	viewer_t::Instance().set_resolution( document_t::Instance().composition().subsample());
	update_recent_files_menu( p);
	ui::user_interface_t::Instance().update();
    }
    catch( boost::archive::archive_exception& e)
    {
	QMessageBox::warning( this, "Error opening document", "Old version or corrupted composition");
	user_interface_t::Instance().create_new_document();
    }
}

void main_window_t::open_document()
{
    if( can_close_document())
    {
	QString fname = QFileDialog::getOpenFileName( 0, tr( "Open Composition"), QString::null, "Ramen Composition (*.rmn)");

	if( !( fname.isEmpty()))
	{
	    user_interface_t::Instance().create_new_document();
	    boost::filesystem::path p( fname.toStdString());
	    open_document( p);
	}
    }
}

void main_window_t::open_recent_document()
{
    if( can_close_document())
    {
	QAction *action = qobject_cast<QAction *>(sender());

	if( action)
	{
	    user_interface_t::Instance().create_new_document();
	    QString fname = action->data().toString();
	    boost::filesystem::path p( fname.toStdString());
	    open_document( p);
	}
    }
}

void main_window_t::save_document()
{
    if( document_t::Instance().has_file())
    {
	document_t::Instance().save();
	undo::undo_stack_t::Instance().clear();
	update_recent_files_menu( document_t::Instance().file());
	ui::user_interface_t::Instance().update();
    }
    else
	save_document_as();
}

void main_window_t::save_document_as()
{
    QString fname = QFileDialog::getSaveFileName( 0, tr( "Save Composition As"), QString::null, "Ramen Composition (*.rmn)");

    if( !(fname.isEmpty()))
    {
	boost::filesystem::path p( fname.toStdString());

	if( p.extension() == std::string())
	    p.replace_extension( ".rmn");

	document_t::Instance().set_file( p);
	document_t::Instance().save();
	undo::undo_stack_t::Instance().clear();
	update_recent_files_menu( p);
	ui::user_interface_t::Instance().update();
    }
}

void main_window_t::export_selection()
{
    QString fname = QFileDialog::getSaveFileName( 0, tr( "Save Composition As"), QString::null, "Ramen Composition (*.rmn)");

    if( !(fname.isEmpty()))
    {
	boost::filesystem::path p( fname.toStdString());

	if( p.extension() == std::string())
	    p.replace_extension( ".rmn");

	export_selected_nodes( p);
    }
}

void main_window_t::import_composition()
{
    QString fname = QFileDialog::getOpenFileName( 0, tr( "Open Composition"), QString::null, "Ramen Composition (*.rmn)");

    if( !( fname.isEmpty()))
    {
	try
	{
	    boost::filesystem::path p( fname.toStdString());
	    ramen::import_composition( p);
	    document_t::Instance().set_dirty( true);
	    undo::undo_stack_t::Instance().clear();
	    ui::user_interface_t::Instance().update();
	}
	catch( boost::archive::archive_exception& e)
	{
	    QMessageBox::warning( this, "Error opening document",
				    "Old version or corrupted composition");

	    user_interface_t::Instance().create_new_document();
	}
    }
}

void main_window_t::import_multilayer_exr()
{
    boost::filesystem::path p;
    bool relative, sequence;

    if( ui::user_interface_t::Instance().image_sequence_file_selector( "Import EXR", "OpenEXR image (*.exr)",
								      p, relative, sequence))
    {
	imageio::import_multichannel_exr( p, relative, sequence);
    }
}

void main_window_t::quit()
{
    if( document_t::Instance().dirty())
    {
        int r = QMessageBox::warning( this, tr( "Ramen"),
                        tr( "The composition has been modified.\n"
                            "Do you want to save your changes before quitting?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);

	switch( r)
	{
	case QMessageBox::Yes:
	{
	    save_document();

            // if the document is still dirty, it means
            // save was cancelled, so we return without quitting
	    if( document_t::Instance().dirty())
                return;
	}
	break;

	case QMessageBox::No:
	    user_interface_t::Instance().quit();
	break;

	case QMessageBox::Cancel:
	return;
	}
    }

    user_interface_t::Instance().quit();
}

void main_window_t::undo()
{
    undo::undo_stack_t::Instance().undo();
    user_interface_t::Instance().update();
    inspector_t::Instance().update();
    anim_editor_t::Instance().update();
}

void main_window_t::redo()
{
    undo::undo_stack_t::Instance().redo();
    user_interface_t::Instance().update();
    inspector_t::Instance().update();
    anim_editor_t::Instance().update();
}

void main_window_t::ignore_nodes()
{
    std::auto_ptr<undo::ignore_nodes_command_t> c( new undo::ignore_nodes_command_t());

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( n.selected())
	    c->add_node( &n);
    }

    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void main_window_t::delete_nodes()
{
    if( !document_t::Instance().any_selected())
	return;

    std::auto_ptr<undo::delete_command_t> c( new undo::delete_command_t());

    BOOST_FOREACH( composition_t::edge_type& e, document_t::Instance().composition().edges())
    {
	if( e.src->selected() && !(e.dst->selected()))
	    c->add_dependent_node( e.dst);
    }

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( n.selected())
	    c->add_node( &n);
    }

    BOOST_FOREACH( composition_t::edge_type& e, document_t::Instance().composition().edges())
    {
	if( e.src->selected() || e.dst->selected())
	    c->add_edge( e);
    }

    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void main_window_t::duplicate_nodes()
{
    std::map<node_t*, node_t*> relation;
    std::auto_ptr<undo::duplicate_command_t> c( new undo::duplicate_command_t());

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( n.selected())
	{
	    node_t *nclone = new_clone( n);
	    nclone->offset_location( Imath::V2f( 20, 20));
	    c->add_node( nclone);
	    relation[&n] = nclone;
	}
    }

    BOOST_FOREACH( composition_t::edge_type& e, document_t::Instance().composition().edges())
    {
	if( e.src->selected() && e.dst->selected())
	    c->add_edge( composition_t::edge_type( relation[e.src], relation[e.dst], e.port));
    }

    document_t::Instance().deselect_all();
    c->redo();
    undo::undo_stack_t::Instance().push_back( c);
    ui::user_interface_t::Instance().update();
}

void main_window_t::clear_cache()
{
    image::image_cache_t::Instance().clear();
}

void main_window_t::show_composition_settings_dialog()
{
    composition_settings_dialog_t::Instance().exec_dialog();
}

struct render_callback
{
    render_callback( QProgressDialog *p) : progress_( p) {}

    bool operator()( int x)
    {
	progress_->setValue( x);
	qApp->processEvents();
	return progress_->wasCanceled();
    }

private:

    QProgressDialog *progress_;
};

void main_window_t::render_composition()
{
    bool any_output = false;
    bool any_output_selected = false;

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( dynamic_cast<image_output_node_t*>( &n))
	{
	    any_output = true;

	    if( n.selected())
		any_output_selected = true;
	}
    }

    if( !any_output)
    {
	QMessageBox::warning( this, tr( "Ramen"), tr( "No output nodes in composition"));
	return;
    }

    render_dialog_t::Instance().set_any_output_selected( any_output_selected);

    int result = render_dialog_t::Instance().exec();

    if( result == QDialog::Accepted)
    {
	int num_frames = render::total_frames_to_render( document_t::Instance().composition(),
							render_dialog_t::Instance().start_frame(),
							render_dialog_t::Instance().end_frame(),
							render_dialog_t::Instance().selected_only());
	
	QProgressDialog progress( "Rendering", "Cancel", 0, num_frames, user_interface_t::Instance().main_window());
	progress.setWindowModality( Qt::ApplicationModal);
	progress.setWindowTitle( "Rendering");
	progress.show();

	render::render_sequence( document_t::Instance().composition(), 
				    render_dialog_t::Instance().start_frame(),
				    render_dialog_t::Instance().end_frame(),
				    render_dialog_t::Instance().resolution(),
                                    render_dialog_t::Instance().mblur_extra_samples(),
                                    render_dialog_t::Instance().mblur_shutter_factor(),
                                    render_dialog_t::Instance().selected_only(),
				    render_callback( &progress));
	
	if( !progress.wasCanceled())
	    progress.setValue( num_frames);
    }
}

void main_window_t::render_flipbook()
{
    int result = render_flipbook_dialog_t::Instance().exec();

    if( result == QDialog::Accepted)
    {
	node_t *n = document_t::Instance().selected_node();
	flipbook::flipbook_t *f = new flipbook::flipbook_t();
	f->setWindowTitle( "Flipbook");
	f->set_frame_rate( document_t::Instance().composition().frame_rate());

	int start	= render_flipbook_dialog_t::Instance().start_frame();
	int end	= render_flipbook_dialog_t::Instance().end_frame();
	int subsample =  render_flipbook_dialog_t::Instance().resolution();

	if( render::render_flipbook( f, n, start, end, viewer_t::Instance().mblur_active(), subsample))
	    f->play();
    }
}

void main_window_t::show_preferences_dialog()
{
    preferences_dialog_t::Instance().exec_dialog();
}

void main_window_t::create_node( QAction *action)
{
    std::string id( create_node_actions_[action]);
    std::auto_ptr<node_t> p = node_factory_t::Instance().create_by_id( id, true);

    if( p.get())
    {
	p->set_composition( &( document_t::Instance().composition()));
	p->create_params();
        p->create_manipulators();

        // test to see if we can autoconnect
        node_t *src = document_t::Instance().selected_node();

        if( src && src->has_output_plug() && p->num_inputs() != 0)
            composition_view()->place_node_near_node( p.get(), src);
        else
        {
            src = 0;
            composition_view()->place_node( p.get());
        }

	node_t *n = p.get(); // save for later use
	std::auto_ptr<undo::command_t> c( new undo::add_node_command_t( p, src));
	document_t::Instance().deselect_all();
	n->select( true);
	c->redo();

	undo::undo_stack_t::Instance().push_back( c);
	ui::user_interface_t::Instance().update();
    }
}

void main_window_t::show_about_box()
{
    // placeholder for a while
    QMessageBox::about( this, "About Ramen", "Ramen 0.6.3. Under the CDDL license.");
}

void main_window_t::go_to_project_website()
{
    QDesktopServices::openUrl( QUrl( "http://ramenhdr.sourceforge.net/"));
}

void main_window_t::update()
{
    if( document_t::Instance().dirty())
	setWindowTitle( tr( "Ramen *"));
    else
	setWindowTitle( tr( "Ramen"));

    update_menus();
    time_slider_->update( document_t::Instance().composition().start_frame(),
			 document_t::Instance().composition().frame(),
			 document_t::Instance().composition().end_frame());

    composition_view()->update();
    vcr_controls_t::Instance().update();
}

void main_window_t::update_recent_files_menu( const boost::filesystem::path& p)
{
    QString fileName( p.external_file_string().c_str());

    QSettings settings( "cddl.ramen", "Ramen Recent Files");
    QStringList files = settings.value( "recent_file_list").toStringList();
    files.removeAll( fileName);
    files.prepend( fileName);
    while( files.size() > max_recently_opened_files)
        files.removeLast();

    settings.setValue("recent_file_list", files);

    int num_recent_files = std::min( files.size(), (int) max_recently_opened_files);

    for( int i = 0; i < num_recent_files; ++i)
    {
	QString stripped = QFileInfo( files[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg( stripped);
        recently_opened_[i]->setText( text);
        recently_opened_[i]->setData( files[i]);
        recently_opened_[i]->setVisible( true);
    }

    for( int j = num_recent_files; j < max_recently_opened_files; ++j)
        recently_opened_[j]->setVisible( false);
}

void main_window_t::init_recent_files_menu()
{
    QSettings settings( "cddl.ramen", "Ramen Recent Files");
    QStringList files = settings.value( "recent_file_list").toStringList();

    int num_recent_files = std::min( files.size(), (int) max_recently_opened_files);

    for( int i = 0; i < num_recent_files; ++i)
    {
	QString stripped = QFileInfo( files[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg( stripped);
        recently_opened_[i]->setText( text);
        recently_opened_[i]->setData( files[i]);
        recently_opened_[i]->setVisible( true);
    }

    for( int j = num_recent_files; j < max_recently_opened_files; ++j)
        recently_opened_[j]->setVisible( false);
}

void main_window_t::update_menus()
{
    bool any_selected = document_t::Instance().any_selected();

    export_sel_->setEnabled( any_selected);

    undo_->setEnabled( !undo::undo_stack_t::Instance().undo_empty());
    redo_->setEnabled( !undo::undo_stack_t::Instance().redo_empty());

    ignore_->setEnabled( any_selected);

    delete_->setEnabled( any_selected);
    duplicate_->setEnabled( any_selected);
    comp_flipbook_->setEnabled( any_selected);
}

} // namespace
} // namespace
