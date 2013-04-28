// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/undo/undo_stack.hpp>

namespace ramen
{
namespace undo
{

undo_stack_impl::undo_stack_impl() {}
undo_stack_impl::~undo_stack_impl() {}

void undo_stack_impl::clear()
{
    undo_stack_.clear();
    redo_stack_.clear();
}

void undo_stack_impl::erase_last_command() { undo_stack_.pop_back();}

void undo_stack_impl::undo()
{
    assert( !undo_stack_.empty());
	
    undo_stack_.back().undo();
    boost::ptr_deque<command_t>::auto_type c( undo_stack_.pop_back());
    redo_stack_.push_back( c.release());
}

void undo_stack_impl::redo()
{
    assert( !redo_stack_.empty());

    redo_stack_.back().redo();
    boost::ptr_deque<command_t>::auto_type c( redo_stack_.pop_back());
    undo_stack_.push_back( c.release());
}

} // namespace
} // namespace
