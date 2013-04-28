// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_HPP
#define	RAMEN_UI_ANIM_EDITOR_HPP

#include<vector>
#include<map>
#include<memory>

#include<boost/shared_ptr.hpp>

#include<loki/Singleton.h>

#include<ramen/Qr/QrUtilityWindow.hpp>

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/curve_fwd.hpp>

#include<ramen/ui/anim/anim_editor_command.hpp>

#include<ramen/anim/keyframe.hpp>

class QSplitter;
class QTreeView;
class QItemSelection;

namespace ramen
{

namespace ui
{

class anim_curves_view_t;
class track_model_t;
class anim_editor_toolbar_t;

class anim_editor_impl : public QObject
{
    Q_OBJECT

public:

    QWidget *widget() { return window_;}

    void show();
    void hide();
    void toggle();

    void set_active_node( node_t *n);

    void update();
    void update_tree();

    void node_released( node_t *n);

    QTreeView *tree() { return tree_;}

    boost::shared_ptr<anim::track_t> tracks_for_node( node_t *n);

    anim::track_t *node_track();
    anim::track_t *selected_track();

    void recreate_tracks( node_t *n);

    const std::vector<anim::curve_t*>& curves() const	{ return curves_;}
    std::vector<anim::curve_t*>& curves()		{ return curves_;}

    anim_editor_toolbar_t *toolbar() { return toolbar_;}

    anim::track_t *find_track( anim::curve_t *c);

    void deselect_all_keyframes();

    void delete_selected_keyframes();

    std::auto_ptr<undo::anim_editor_command_t> create_command();
    std::auto_ptr<undo::anim_editor_command_t> create_command( anim::curve_t *c);

    void set_tangents_smooth();
    void set_tangents_flat();
    void set_tangents_linear();
    void set_tangents_step();

    void set_extrapolation_constant();
    void set_extrapolation_linear();
    void set_extrapolation_repeat();

    void frame_selection();
    void frame_all();

private Q_SLOTS:

    void update_selection( const QItemSelection& selected, const QItemSelection& deselected);

private:

    anim_editor_impl();
    ~anim_editor_impl();

    void create_tracks( node_t *n);

    void insert_in_curves( anim::curve_t& curve);

    void set_selected_keys_autotangents( anim::auto_tangent_method m);

    void set_extrapolation( anim::extrapolation_method e);

    friend struct Loki::CreateUsingNew<anim_editor_impl>;

    QrUtilityWindow *window_;
    anim_editor_toolbar_t *toolbar_;
    QSplitter *split_;
    QTreeView *tree_;
    anim_curves_view_t *view_;

    track_model_t *default_model_;

    std::map<node_t*, track_model_t*> tracks_;
    std::map<node_t*, track_model_t*>::iterator current_;

    std::vector<anim::curve_t*> curves_;
};

typedef Loki::SingletonHolder<anim_editor_impl> anim_editor_t;

} // namespace
} // namespace

#endif
