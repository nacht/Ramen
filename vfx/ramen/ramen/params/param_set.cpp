// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/param_set.hpp>

#include<stdio.h>

#include<stdexcept>
#include<algorithm>
#include<sstream>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/ptr_container/ptr_map.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/params/parameterised.hpp>
#include<ramen/params/param.hpp>
#include<ramen/params/composite_param.hpp>

#include<ramen/undo/undo_stack.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/viewer.hpp>

#include<iostream>

namespace ramen
{

class param_set_command_t : public undo::command_t
{
public:

    param_set_command_t( const std::string& name) : command_t( name) {}

    bool empty() const { return commands_.empty();}

    bool has_command_for_param( param_t *p) { return commands_.find( p) != commands_.end();}

    void add_command( param_t *p, std::auto_ptr<undo::command_t> c) { commands_.insert( p, c);}

    virtual void undo()
    {
	param_set_t *pset = commands_.begin()->first->param_set();

	for( boost::ptr_map<param_t*, undo::command_t>::iterator it( commands_.begin()); it != commands_.end(); ++it)
	    it->second->undo();

	pset->notify_parent();
	undo::command_t::undo();
    }

    virtual void redo()
    {
	param_set_t *pset = commands_.begin()->first->param_set();

	for( boost::ptr_map<param_t*, undo::command_t>::iterator it( commands_.begin()); it != commands_.end(); ++it)
	    it->second->redo();

	pset->notify_parent();
	undo::command_t::redo();
    }

private:

    boost::ptr_map<param_t*, undo::command_t> commands_;
};

param_set_t::param_set_t( parameterised_t *p) : parent_( p) {}

param_set_t::param_set_t( const param_set_t& other) : params_( other.params_)
{
    parent_ = 0;
    adobe::for_each( params_, boost::bind( &param_set_t::add_param_to_map, this, _1));
    adobe::for_each( params_, boost::bind( &param_t::set_param_set, _1, this));
}

param_set_t::~param_set_t()
{
    // empty destructor to allow auto_prt to use an incomplete type.
    // Do not remove.
}

void param_set_t::clear()
{
    command_.reset();
    param_map_.clear();
    params_.clear();
}

void param_set_t::add_param_to_map( param_t& p)
{
    // if the id string is not empty, add it to the map
    if( !p.id().empty())
    {
        std::map<std::string, param_t*>::const_iterator it( param_map_.find( p.id()));

        if( it != param_map_.end())
        {
            std::cout <<  "Duplicated param id: " << p.id() << " error\n";
            abort();
        }

        param_map_[p.id()] = &p;
    }

    composite_param_t *cp = dynamic_cast<composite_param_t*>( &p);

    if( cp)
	adobe::for_each( cp->params(), boost::bind( &param_set_t::add_param_to_map, this, _1));
}

const param_t& param_set_t::find( const std::string id) const
{
    std::map<std::string, param_t*>::const_iterator it( param_map_.find( id));

    if( it == param_map_.end())
	throw std::runtime_error( std::string( "Param not found: ").append( id));

    return *(it->second);
}

param_t& param_set_t::find( const std::string id)
{
    std::map<std::string, param_t*>::iterator it( param_map_.find( id));

    if( it == param_map_.end())
	throw std::runtime_error( std::string( "Param not found: ").append( id));

    return *(it->second);
}

void param_set_t::notify_parent()
{
    if( parent())
        parent()->param_edit_finished();
}

void param_set_t::begin_edit()
{
    command_.reset( new param_set_command_t( "Params changed"));
}

void param_set_t::end_edit( bool notify)
{
    if( command_.get() && !is_command_empty())
	undo::undo_stack_t::Instance().push_back( command_);
    else
	command_.reset();

    if( notify)
	notify_parent();

    ui::anim_editor_t::Instance().update();
}

bool param_set_t::editing() const { return command_.get();}

bool param_set_t::is_command_empty() const
{
    if( !command_.get())
	return true;

    return command_->empty();
}

void param_set_t::add_command( param_t *p)
{
    if( editing())
    {
	if( !command_->has_command_for_param( p))
	{
	    std::auto_ptr<undo::command_t> c( p->create_command());

	    if( c.get())
		command_->add_command( p, c);
	}
   }
}

std::auto_ptr<undo::command_t> param_set_t::create_reset_command()
{
    std::auto_ptr<undo::composite_command_t> c( new undo::composite_command_t( "Reset"));

    BOOST_FOREACH( param_t& p, params_)
    {
        std::auto_ptr<undo::command_t> q( p.create_reset_command());

        if( q.get())
            c->push_back( q);
    }

    if( c->empty())
        return std::auto_ptr<undo::command_t>();

    return std::auto_ptr<undo::command_t>( c.release());
}

bool param_set_t::autokey() const
{
    if( parent())
	return parent()->autokey();
    else
	return false;
}

void param_set_t::param_values_map( std::map<std::string, adobe::any_regular_t>& map) const
{
    for( std::map<std::string, param_t*>::const_iterator it( param_map_.begin()); it != param_map_.end(); ++it)
    {
	if( it->second->has_value())
	    map[it->first] = it->second->value();
    }
}

void param_set_t::set_param_values( const std::map<std::string, adobe::any_regular_t>& map)
{
    for( std::map<std::string, param_t*>::iterator it( param_map_.begin()); it != param_map_.end(); ++it)
    {
	if( it->second->has_value())
	{
	    std::map<std::string, adobe::any_regular_t>::const_iterator value_it( map.find( it->second->id()));

	    if( value_it != map.end())
	    {
		if( it->second->value().type_info() == value_it->second.type_info())
		{
		    it->second->value().assign( value_it->second);
                    it->second->update_widgets();
		}
	    }
	}
    }
}

std::string param_set_t::str() const
{
    std::stringstream s;

    BOOST_FOREACH( const param_t& p, params())
    {
        s << p.str();
    }

    return s.str();
}

void param_set_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & boost::serialization::make_nvp( "params", params_);
    adobe::for_each( params_, boost::bind( &param_set_t::add_param_to_map, this, _1));
    adobe::for_each( params_, boost::bind( &param_t::set_param_set, _1, this));
}

void param_set_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & boost::serialization::make_nvp( "params", params_);
}

} // namespace
