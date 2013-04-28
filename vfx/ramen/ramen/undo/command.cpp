// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/undo/command.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace undo
{

command_t::command_t( const std::string& name) : name_(name), was_dirty_( document_t::Instance().dirty()) {}
command_t::~command_t() {}

const std::string& command_t::name() const { return name_;}

void command_t::set_done( bool b)
{
    done_ = b;

    if( done_)
	document_t::Instance().set_dirty( true);
}

void command_t::undo()
{ 
    document_t::Instance().set_dirty( was_dirty_);
    done_ = false;
}

void command_t::redo()
{ 
    document_t::Instance().set_dirty( true);
    done_ = true;
}

generic_command_t::generic_command_t( const std::string& name, const function_type& undo_fun, const function_type& redo_fun) : command_t( name)
																															 , undo_(undo_fun)
																															 , redo_(redo_fun)
{
}

void generic_command_t::undo()
{
	undo_();
	command_t::undo();
}

void generic_command_t::redo()
{
	redo_();
	command_t::redo();
}

composite_command_t::composite_command_t( const std::string& name) : command_t( name) {}

void composite_command_t::undo()
{
	adobe::for_each( commands_, boost::bind( &command_t::undo, _1));
	command_t::undo();
}

void composite_command_t::redo()
{
	adobe::for_each( commands_, boost::bind( &command_t::redo, _1));
	command_t::redo();
}
	
void composite_command_t::push_back( std::auto_ptr<command_t> c) { commands_.push_back( c);}

} // namespace
} // namespace
