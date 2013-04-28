// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QHBoxLayout>
#include<QSplitter>
#include<QTreeView>
#include<QComboBox>

#include<ramen/Qr/QrUtilityWindow.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/track_model.hpp>
#include<ramen/ui/anim/anim_curves_view.hpp>

#include<ramen/ui/anim/anim_editor_toolbar.hpp>

#include<ramen/undo/undo_stack.hpp>

namespace
{

struct find_track_fun
{
    find_track_fun( ramen::anim::curve_t *c) : curve(c), result(0) {}

    void operator()( ramen::anim::track_t *t)
    {
	if( t->curve() == curve)
	    result = t;
    }

    ramen::anim::curve_t *curve;
    ramen::anim::track_t *result;
};

} // unnamed

namespace ramen
{
namespace ui
{

anim_editor_impl::anim_editor_impl() : window_(0), current_( tracks_.end())
{
    window_ = new QrUtilityWindow();
    window_->setWindowTitle( QObject::tr( "anim_editor"));
    window_->move( 200, 550);
    window_->resize( 715, 300);

    QVBoxLayout *vlayout = new QVBoxLayout();

    vlayout->setContentsMargins( 0, 0, 0, 0);

    toolbar_ = new anim_editor_toolbar_t();
    vlayout->addWidget( toolbar_);

    split_ = new QSplitter();

    tree_ = new QTreeView();
    tree_->setModel( 0);
    split_->addWidget( tree_);

    view_ = new anim_curves_view_t();
    split_->addWidget( view_);

    vlayout->addWidget( split_);

    default_model_ = new track_model_t();
    tree_->setModel( default_model_);

    window_->setLayout( vlayout);
}

anim_editor_impl::~anim_editor_impl() { default_model_->deleteLater();}

void anim_editor_impl::show()	{ window_->show();}
void anim_editor_impl::hide()	{ window_->hide();}
void anim_editor_impl::toggle()	{ window_->toggle();}

void anim_editor_impl::set_active_node( node_t *n)
{
    curves_.clear();

    QItemSelectionModel *sel_model = tree_->selectionModel();

    if( n != 0)
    {
	std::map<node_t*,track_model_t*>::const_iterator it = tracks_.find( n);

	if( it == tracks_.end())
	    create_tracks( n);

	current_ = tracks_.find( n);
	tree_->setModel( current_->second);

	QItemSelectionModel *new_sel = tree_->selectionModel();
	connect( new_sel, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection&)),
		this, SLOT( update_selection( const QItemSelection&, const QItemSelection&)));
    }
    else
    {
	tree_->setModel( default_model_);
	current_ = tracks_.end();
    }

    delete sel_model;

    if( !user_interface_t::Instance().quitting())
	view_->update();
}

void anim_editor_impl::node_released( node_t *n)
{
    std::map<node_t*, track_model_t*>::iterator it( tracks_.find( n));

    if( it != tracks_.end())
    {
	it->second->deleteLater();
	tracks_.erase( n);
    }
}

void anim_editor_impl::create_tracks( node_t *n)
{
    track_model_t *model = new track_model_t();
    n->create_tracks( model->root());
    tracks_[n] = model;
}

void anim_editor_impl::recreate_tracks( node_t *n)
{
    if( n == user_interface_t::Instance().active_node())
    {
	set_active_node( 0);
	node_released( n);
	set_active_node( n);
    }
    else
	node_released( n);
}

anim::track_t *anim_editor_impl::find_track( anim::curve_t *c)
{
    // This is suboptimal in almost every way possible
    // We need a better way to do this
    find_track_fun f( c);
    anim::for_each_leaf_track( node_track(), boost::bind<void>( boost::ref( f), _1));
    return f.result;
}

void anim_editor_impl::update_selection( const QItemSelection& selected, const QItemSelection& deselected)
{
    curves_.clear();

    anim::track_t *track = anim_editor_t::Instance().selected_track();

    if( track)
	anim::for_each_curve( track, boost::bind( &anim_editor_impl::insert_in_curves, this, _1));

    view_->update();
}

boost::shared_ptr<anim::track_t> anim_editor_impl::tracks_for_node( node_t *n)
{
    std::map<node_t*, track_model_t*>::iterator it( tracks_.find( n));

    if( it != tracks_.end())
	return it->second->tracks();

    return boost::shared_ptr<anim::track_t>();
}

anim::track_t *anim_editor_impl::node_track()
{
    if( user_interface_t::Instance().active_node())
	return current_->second->root()->child( 0);
    else
	return 0;
}

anim::track_t *anim_editor_impl::selected_track()
{
    QItemSelectionModel *selection = tree()->selectionModel();
    QModelIndexList indexes = selection->selectedIndexes();

    if( indexes.empty())
	return 0;

    assert( indexes.size() == 1);
    return static_cast<anim::track_t*>( indexes.front().internalPointer());
}

std::auto_ptr<undo::anim_editor_command_t> anim_editor_impl::create_command()
{
    std::auto_ptr<undo::anim_editor_command_t> cmd( new undo::anim_editor_command_t());

    BOOST_FOREACH( anim::curve_t *c, curves())
    {
	if( c->any_keyframe_selected())
	{
	    anim::track_t *t = find_track( c);
	    cmd->add_track( t);
	}
    }

    return cmd;
}

std::auto_ptr<undo::anim_editor_command_t> anim_editor_impl::create_command( anim::curve_t *c)
{
    std::auto_ptr<undo::anim_editor_command_t> cmd( new undo::anim_editor_command_t());
    anim::track_t *t = find_track( c);
    cmd->add_track( t);
    return cmd;
}

void anim_editor_impl::deselect_all_keyframes()
{
    adobe::for_each( curves(), boost::bind( &anim::curve_t::select_all_keyframes, _1, false));
}

void anim_editor_impl::delete_selected_keyframes()
{
    std::auto_ptr<undo::anim_editor_command_t> cmd( new undo::anim_editor_command_t());

    BOOST_FOREACH( anim::curve_t *c, curves())
    {
	if( c->any_keyframe_selected())
	{
	    anim::track_t *t = find_track( c);
	    cmd->add_track( t);
	    c->erase_selected_keyframes();
	    c->recalc_tangents_and_coefficients();
	    t->notify();
	}
    }

    if( !cmd->empty())
    {
	undo::undo_stack_t::Instance().push_back( cmd);
	user_interface_t::Instance().active_node()->notify();
	user_interface_t::Instance().active_node()->notify_dependents();
	user_interface_t::Instance().update();
	view_->update();
    }
}

void anim_editor_impl::set_tangents_smooth()
{
    set_selected_keys_autotangents( anim::tangent_smooth);
}

void anim_editor_impl::set_tangents_flat()
{
    set_selected_keys_autotangents( anim::tangent_flat);
}

void anim_editor_impl::set_tangents_linear()
{
    set_selected_keys_autotangents( anim::tangent_linear);
}

void anim_editor_impl::set_tangents_step()
{
    set_selected_keys_autotangents( anim::tangent_step);
}

void anim_editor_impl::insert_in_curves( anim::curve_t& curve) { curves_.push_back( &curve);}

void anim_editor_impl::set_selected_keys_autotangents( anim::auto_tangent_method m)
{
    if( !user_interface_t::Instance().active_node())
	return;

    std::auto_ptr<undo::anim_editor_command_t> cmd( create_command());

    BOOST_FOREACH( anim::curve_t *c, curves())
    {
	if( c->any_keyframe_selected())
	{
	    BOOST_FOREACH( anim::keyframe_t& k, c->keys())
	    {
		if( k.selected())
		{
		    k.set_tangent_continuity( false);

		    if( m == anim::tangent_step)
		    {
			k.set_v0_auto_tangent( anim::tangent_fixed);
			k.set_v1_auto_tangent( m);
		    }
		    else
		    {
			k.set_v0_auto_tangent( m);
			k.set_v1_auto_tangent( m);

			if( m == anim::tangent_smooth || m == anim::tangent_flat)
			    k.set_tangent_continuity( true);
		    }
		}
	    }

	    c->recalc_tangents_and_coefficients();
	}
    }

    if( !cmd->empty())
    {
        BOOST_FOREACH( anim::track_t *t, cmd->tracks())
	{
	    t->notify();
	}
	
	undo::undo_stack_t::Instance().push_back( cmd);
	user_interface_t::Instance().active_node()->notify();
	user_interface_t::Instance().active_node()->notify_dependents();
	user_interface_t::Instance().update();
	view_->update();
    }
}

void anim_editor_impl::set_extrapolation_constant()
{
    set_extrapolation( anim::extrapolate_constant);
}

void anim_editor_impl::set_extrapolation_linear()
{
    set_extrapolation( anim::extrapolate_linear);
}

void anim_editor_impl::set_extrapolation_repeat()
{
    set_extrapolation( anim::extrapolate_repeat);
}

void anim_editor_impl::set_extrapolation( anim::extrapolation_method e)
{
    if( !user_interface_t::Instance().active_node())
	return;

    std::auto_ptr<undo::anim_editor_command_t> cmd( create_command());

    BOOST_FOREACH( anim::curve_t *c, curves())
    {
	c->set_extrapolation( e);
    }

    if( !cmd->empty())
    {
        BOOST_FOREACH( anim::track_t *t, cmd->tracks())
	{
	    t->notify();
	}

	undo::undo_stack_t::Instance().push_back( cmd);
	user_interface_t::Instance().active_node()->notify();
	user_interface_t::Instance().active_node()->notify_dependents();
	user_interface_t::Instance().update();
	view_->update();
    }
}

void anim_editor_impl::frame_selection()
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::curve_t *c, curves())
    {
        bounds.extendBy( c->selection_bounds());
    }

    if( !bounds.isEmpty())
    {
	view_->frame_area( bounds);
	view_->update();
    }
}

void anim_editor_impl::frame_all()
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::curve_t *c, curves())
    {
	bounds.extendBy( c->bounds());
    }

    if( !bounds.isEmpty())
    {
	view_->frame_area( bounds);
	view_->update();
    }
}

void anim_editor_impl::update()
{
    if( !user_interface_t::Instance().quitting())
    {
	tree_->update();
	view_->update();
    }
}

void anim_editor_impl::update_tree()
{
    if( !user_interface_t::Instance().quitting())
	tree_->update();
}

} // namespace
} // namespace
