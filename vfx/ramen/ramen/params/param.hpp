/**
 * @file
 * @brief Base param class
 *
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAM_HPP
#define RAMEN_PARAM_HPP

#include<memory>
#include<sstream>
#include<string>

#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>

#include<boost/serialization/access.hpp>
#include<boost/serialization/base_object.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/serialization/export.hpp>
#include<boost/serialization/string.hpp>

#include<adobe/any_regular.hpp>

#include<OpenEXR/ImathBox.h>

#include<QObject>

#include<ramen/params/parameterised_fwd.hpp>
#include<ramen/params/param_set_fwd.hpp>

#include<ramen/app/composition_fwd.hpp>

#include<ramen/anim/track_fwd.hpp>

#include<ramen/undo/command.hpp>
#include<ramen/params/static_param_command_fwd.hpp>
#include<ramen/params/animated_param_command_fwd.hpp>

#include<ramen/serialization/any_regular.hpp>

#include<ramen/ui/inspector.hpp>

class QWidget;

namespace ramen
{

/**
* @class param_t
* @brief Base class for params
*/

class param_t : public QObject
{
    Q_OBJECT

protected:

    enum flag_bits
    {
        dirty_bit               = 1 << 0,   // currently unused, could be recycled...
        static_bit              = 1 << 1,   // param does not animate.
	secret_bit              = 1 << 2,   // param does not show on the inspector.
	persist_bit             = 1 << 3,   // param value is not saved to file.
	can_undo_bit            = 1 << 4,   // param uses undo.
	enabled_bit             = 1 << 5,   // param widgets are enabled in the inspector.
	dont_track_mouse_bit    = 1 << 6,   // node is processed on mouse up only.
        round_to_int_bit        = 1 << 7,   // for float params, round to value to integer. Emulates an integer_param.
        proportional_bit        = 1 << 8    // param is a float2 or float3 and the values can be modified proportionally.
    };

public:
    
    param_t();
    explicit param_t( const std::string& name);
    virtual ~param_t() {}

    void init();

    param_t *clone() const { return do_clone();}

    // this is for OFX
    virtual void copy( const param_t& other, float offset = 0.0f);
    virtual void copy( const param_t& other, float offset, float start, float end);

    /// Get the name of the param
    const std::string& name() const	    { return name_;}

    /** Set the name of the param
     * @param name The new name
     */
    void set_name( const std::string& name) { name_ = name;}

    /// Get the id of the param
    const std::string& id() const		{ return id_;}

    /** Set the name of the param
     * @param id The new id
     */
    void set_id( const std::string& identifier) { id_ = identifier;}

    const param_set_t *param_set() const    { return param_set_;}
    param_set_t *param_set()		    { return param_set_;}

    virtual void set_param_set( param_set_t *parent);

    const parameterised_t *parameterised() const;
    parameterised_t *parameterised();

    const composition_t *composition() const;
    composition_t *composition();

    bool enabled() const { return flags_ & enabled_bit;}
    void set_enabled( bool e);

    bool is_static() const      { return flags_ & static_bit;}
    void set_static( bool b);

    bool secret() const		{ return flags_ & secret_bit;}
    void set_secret( bool b);

    bool persist() const	{ return flags_ & persist_bit;}
    void set_persist( bool b);

    bool can_undo() const	{ return flags_ & can_undo_bit;}
    void set_can_undo( bool b);

    bool track_mouse() const        { return !( flags_ & dont_track_mouse_bit);}
    void set_track_mouse( bool b);

    bool has_value() const;

    // notify
    void notify();

    // animation
    void create_tracks( anim::track_t *parent);
    void evaluate( float time);

    // hash
    std::string str() const;

    // widgets
    QWidget *create_widgets();
    void update_widgets()           { do_update_widgets();}
    void enable_widgets( bool e)    { do_enable_widgets( e);}
    
    void domain_changed( const Imath::Box2i& new_domain) { do_domain_changed( new_domain);}

    // undo
    virtual std::auto_ptr<undo::command_t> create_command()         { return std::auto_ptr<undo::command_t>();}
    virtual std::auto_ptr<undo::command_t> create_reset_command()   { return std::auto_ptr<undo::command_t>();}

protected:

    param_t( const param_t& other);
    void operator=( const param_t& other);

    int flags() const		{ return flags_;}
    void set_flags( int flags)	{ flags_ = flags;}

    const adobe::any_regular_t& value() const	{ return value_;}
    adobe::any_regular_t& value()		{ return value_;}

    adobe::any_regular_t value_at_time( float time) const { return do_get_value_at_time( time);}

    const adobe::any_regular_t& default_value() const	{ return default_;}
    adobe::any_regular_t& default_value()		{ return default_;}

    template<class T>
    std::string string_for_type() const
    {
        std::stringstream s;
	adobe::any_regular_t x = value();
	s << x.cast<T>();
	return s.str();
    }

    // serialization
    void serialize_values( boost::archive::polymorphic_iarchive& ar);
    void serialize_values( boost::archive::polymorphic_oarchive& ar) const;

private:

    friend class param_set_t;
    friend class static_param_command_t;
    friend class static_param_reset_command_t;
    friend class animated_param_command_t;
    friend class animated_param_reset_command_t;

    virtual param_t *do_clone() const = 0;

    virtual void do_init() {}

    virtual adobe::any_regular_t do_get_value_at_time( float time) const;

    virtual void do_create_tracks( anim::track_t *parent) {}
    virtual void do_evaluate( float time);

    virtual std::string do_str() const;

    virtual QWidget *do_create_widgets()        { return 0;}
    virtual void do_update_widgets()            {}
    virtual void do_enable_widgets( bool e)     {}

    virtual void do_domain_changed( const Imath::Box2i& new_domain) {}

    template<class S> friend S get_value( const param_t& p);
    template<class S> friend S get_value_at_time( const param_t& p, float time);

    template<class S> friend S get_absolute_value( const param_t& p);
    template<class S> friend S get_absolute_value_at_time( const param_t& p, float time);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    param_set_t *param_set_;

    std::string id_;
    boost::uint32_t flags_;
    std::string name_;
    adobe::any_regular_t value_, default_;
};

/**
 * Get the value of a param as an object of type S
 * @param p The param to get the value
 */
template<class S>
S get_value( const param_t& p)
{ 
    const adobe::any_regular_t& any( p.value());
    return any.cast<S>();
}

/**
 * Get the value at time time of a param as an object of type S
 * @param p the param to get the value
 * @param time time
 */
template<class S>
S get_value_at_time( const param_t& p, float time)
{
    adobe::any_regular_t any( p.value_at_time( time));
    return any.cast<S>();
}

/**
 * Creates a new clone of the param
 * @param other The param to clone
 */
param_t* new_clone( const param_t& other);

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::param_t)

#endif
