/**
 * @file
 * @brief A paramerised object is an object that has
 * a collection of params.
 *
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAMETERISED_HPP
#define	RAMEN_PARAMETERISED_HPP

#include<ramen/params/parameterised_fwd.hpp>

#include<boost/signal.hpp>

#include<ramen/params/param_set.hpp>
#include<ramen/app/composition_fwd.hpp>

#include<ramen/anim/track_fwd.hpp>

namespace ramen
{

/**
 * @class parameterised_t
 * @brief an object that has a collection of params.
 *
 * parameterised_t is an object that has a collection of params.
 *
 */

class parameterised_t
{
public:

    parameterised_t();
    parameterised_t( const parameterised_t& other);

    virtual ~parameterised_t();

    typedef boost::signal<void ( parameterised_t*)> release_signal_type;
    typedef boost::signals::connection connection_type;

    connection_type attach_release_observer( release_signal_type::slot_function_type subscriber)
    {
	return released_.connect( subscriber);
    }

    void detach_release_observer( connection_type subscriber) { subscriber.disconnect();}

    bool dont_persist_params() const        { return dont_persist_params_;}
    void set_dont_persist_params( bool b)   { dont_persist_params_ = b;}

    bool autokey() const        { return autokey_;}
    void set_autokey( bool b)   { autokey_ = b;}

    const composition_t *composition() const            { return composition_;}
    composition_t *composition()                        { return composition_;}
    virtual void set_composition( composition_t *comp)  { composition_ = comp;}

    // params
    void create_params();

    const param_set_t& param_set() const    { return params_;}
    param_set_t& param_set()		    { return params_;}

    const param_t& param( const std::string& identifier) const;
    param_t& param( const std::string& identifier);

    template<class T>
    void add_param( std::auto_ptr<T> p) { param_set().add_param( p);}

    virtual void param_edit_finished() = 0;

    void evaluate( float time);

private:

    virtual void do_create_params();

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    composition_t *composition_;
    param_set_t params_;
    bool dont_persist_params_;
    bool autokey_;
    release_signal_type released_;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::parameterised_t)

#endif
