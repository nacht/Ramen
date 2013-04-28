// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_MAIN_WINDOW_HPP
#define	RAMEN_UI_MAIN_WINDOW_HPP

#include<vector>
#include<map>
#include<string>

#include<boost/filesystem/path.hpp>

#include<QMainWindow>

class QAction;
class QMenu;
class QMenuBar;
class QDockWidget;

// It would be better to use BOOST_STATIC_CONSTANT,
// but it's not working on gcc (linker errors).
#define max_recently_opened_files 5

namespace ramen
{
namespace ui
{

class composition_view_t;
class time_slider_t;

class main_window_t : public QMainWindow
{
    Q_OBJECT

public:

    main_window_t();

    composition_view_t *composition_view()  { return comp_view_;}
    time_slider_t *time_slider()	    { return time_slider_;}

    void update();

    void open_document( const boost::filesystem::path& p);
    bool can_close_document();

protected:

    void closeEvent( QCloseEvent *event);

public Q_SLOTS:

    void new_document();
    void open_document();
    void open_recent_document();

    void save_document();
    void save_document_as();

    void export_selection();
    void import_composition();

    void import_multilayer_exr();

    void quit();

    void undo();
    void redo();

    void ignore_nodes();
    void delete_nodes();
    void duplicate_nodes();

    void clear_cache();

    void show_preferences_dialog();

    void show_composition_settings_dialog();

    void render_flipbook();
    void render_composition();

    void show_about_box();
    void go_to_project_website();

    void create_node( QAction *action);
    
private:

    void create_actions();
    void create_menus();

    std::vector<QMenu*>::iterator find_submenu( const std::string& name);

    void create_node_actions();

    void update_recent_files_menu( const boost::filesystem::path& p);
    void init_recent_files_menu();
    void update_menus();

    time_slider_t *time_slider_;
    composition_view_t *comp_view_;

    QDockWidget *inspector_dock_;
    QDockWidget *composition_dock_;
    QDockWidget *anim_editor_dock_;
    QDockWidget *viewer_dock_;
    QDockWidget *vcr_controls_dock_;
    QDockWidget *py_console_dock_;

    QMenuBar *menubar_;

    QMenu *file_, *edit_, *composition_, *create_, *view_, *help_;
    QMenu *open_recent_;
    QMenu *import_, *export_;

    QAction *recently_opened_[max_recently_opened_files];

    std::vector<QMenu*> create_submenus_;

    QAction *new_, *open_, *save_, *save_as_;
    QAction *quit_;

    QAction *import_comp_, *export_sel_;

    QAction *import_multiexr_;

    QAction *undo_, *redo_, *ignore_, *delete_;
    QAction *duplicate_;
    QAction *clear_cache_, *preferences_;

    QAction *comp_settings_, *comp_flipbook_, *comp_render_;
    
    QAction *about_, *project_web_;

    // non - menu actions
    QAction *next_frame_, *prev_frame_;

    std::map<QAction*,std::string> create_node_actions_;

    std::vector<QMenu*>::iterator find_ofx_submenu( const std::string& name);

    QMenu *create_ofx_;
    std::vector<QMenu*> ofx_submenus_;
};

} // namespace
} // namespace

#endif

