// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DOCUMENT_HPP
#define RAMEN_DOCUMENT_HPP

#include<boost/noncopyable.hpp>
#include<boost/filesystem/path.hpp>

#include<loki/Singleton.h>

#include<ramen/app/composition.hpp>

namespace ramen
{

/**
 * @class document_t
 * @brief document class
 *
 * A document associates a composition with a disk file
 * tracks if the composition needs saving and some other additions.
 *
 */

class document_impl : boost::noncopyable
{
public:

    bool dirty() const	    { return dirty_;}
    void set_dirty( bool d) { dirty_ = d;}

    bool has_file() const				{ return !file_.empty();}

    boost::filesystem::path file() const		{ return file_;}
    void set_file( const boost::filesystem::path& p);

    void load( const boost::filesystem::path& p);
    void save();

    // observer
    typedef boost::signals::connection connection_type;
    typedef boost::signal<void ( composition_t::node_type*)> release_signal_type;

    // called when a node is released, used in the inspector and viewer
    connection_type attach_release_observer( release_signal_type::slot_function_type subscriber)
    {
	return released_.connect( subscriber);
    }

    void detach_release_observer( connection_type subscriber) { subscriber.disconnect();}

    // some forwarding methods
    void add_node( std::auto_ptr<composition_t::node_type> n);
    std::auto_ptr<composition_t::node_type> release_node( composition_t::node_type *n);

    void add_edge( const composition_t::edge_type& e, bool notify = false)	{ composition().add_edge( e, notify);}
    void remove_edge( const composition_t::edge_type& e, bool notify = false)   { composition().remove_edge( e, notify);}

    composition_t& composition()		{ return comp_;}
    const composition_t& composition() const	{ return comp_;}

    // selection
    void deselect_all();
    bool any_selected() const;
    node_t *selected_node();

private:

    document_impl();
    ~document_impl();

    friend struct Loki::CreateUsingNew<document_impl>;

    bool dirty_;
    boost::filesystem::path file_;

    composition_t comp_;

    release_signal_type released_;
};

typedef Loki::SingletonHolder<document_impl, Loki::CreateUsingNew, Loki::DeletableSingleton> document_t;

} // namespace

#endif
