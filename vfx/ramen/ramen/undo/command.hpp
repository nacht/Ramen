// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMMAND_HPP
#define RAMEN_COMMAND_HPP

#include<string>

#include<boost/function.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

namespace ramen
{
namespace undo
{

class command_t
{
public:
	
    command_t( const std::string& name);
    virtual ~command_t();

    const std::string& name() const;

    bool done() const { return done_;}
    void set_done( bool b);

    virtual void undo();
    virtual void redo();

protected:

    std::string name_;
    bool was_dirty_;
    bool done_;
};

class generic_command_t : public command_t
{
public:

    typedef boost::function<void()> function_type;

    generic_command_t( const std::string& name, const function_type& undo_fun, const function_type& redo_fun);
    virtual void undo();
    virtual void redo();

private:

    function_type undo_;
    function_type redo_;
};

class composite_command_t : public command_t
{
public:

    composite_command_t( const std::string& name);

    virtual void undo();
    virtual void redo();

    bool empty() const { return commands_.empty();}

    void push_back( std::auto_ptr<command_t> c);

protected:

    boost::ptr_vector<command_t> commands_;
};

} // namespace
} // namespace

#endif
