/**
 * @file
 * @brief A param set it's a collection of params.
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAM_PARAM_SET_HPP
#define	RAMEN_PARAM_PARAM_SET_HPP

#include<map>
#include<memory>
#include<string>

#include<boost/signal.hpp>

#include<boost/ptr_container/serialize_ptr_vector.hpp>
#include<boost/archive/polymorphic_iarchive.hpp>
#include<boost/archive/polymorphic_oarchive.hpp>
#include<boost/serialization/access.hpp>

#include<ramen/params/param.hpp>
#include<ramen/params/parameterised_fwd.hpp>

namespace ramen
{

class composite_param_t;

class param_set_command_t;

/**
 * @class param_set_t
 * @brief a collection of params.
 *
 * param_set_t is a container for a collection of params.
 *
 */

class param_set_t
{
public:

    param_set_t( parameterised_t *p = 0);
    param_set_t( const param_set_t& other);

    ~param_set_t();

    /** Get the parameterised object this param set belongs to.
     */
    const parameterised_t *parent() const   { return parent_;}
    parameterised_t *parent()		    { return parent_;}

    /** Set the parameterised object this param set belongs to.
     * @param p
     */
    void set_parent( parameterised_t *p)    { parent_ = p;}

    void clear();
    bool empty() const { return params_.empty();}

    // iterator and ranges

    const boost::ptr_vector<param_t>& params() const    { return params_;}
    boost::ptr_vector<param_t>& params()                { return params_;}

    typedef boost::ptr_vector<param_t>::const_iterator const_iterator;
    typedef boost::ptr_vector<param_t>::iterator iterator;

    const_iterator begin() const    { return params_.begin();}
    const_iterator end() const	    { return params_.end();}

    iterator begin()	{ return params_.begin();}
    iterator end()	{ return params_.end();}

    /** Add a param to this param set.
     * Inside a param_set all params must have an unique id.
     * @param p is an auto_ptr to an object derived from param_t
     */
    template<class T>
    void add_param( std::auto_ptr<T> p)
    {
	p->set_param_set( this);
	add_param_to_map( *p);
	params_.push_back( p.release());
    }

    /** Find a param by id inside this param set
     * @param id The id of the param
     */
    const param_t& find( const std::string id) const;

    /** Find a param by id inside this param set
     * @param id The id of the param
     */
    param_t& find( const std::string id);

    boost::signal<void ( param_t*)> param_changed;
    void notify_parent();

    /** Starts an undo command for this param set.
     * @param id The id of the param
     */
    void begin_edit();

    /** Ends the undo command for this param set
     * and adds it to the undo stack.
     * @param notify If notify is true, notify dependent objects
     * that some params changed.
     */
    void end_edit( bool notify = true);

    bool editing() const;

    bool is_command_empty() const;

    void add_command( param_t *p);
    std::auto_ptr<undo::command_t> create_reset_command();

    bool autokey() const;

    void param_values_map( std::map<std::string, adobe::any_regular_t>& map) const;
    void set_param_values( const std::map<std::string, adobe::any_regular_t>& map);

    /** Return a std::string with the current state of
     * all the params inside the param_set.
     * You can hash this string and have a quick way to
     * know if any params have changed.
     */
    std::string str() const;

private:

    friend class composite_param_t;

    void add_param_to_map( param_t& p);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    node_t *node_;
    parameterised_t *parent_;

    boost::ptr_vector<param_t> params_;
    std::map<std::string, param_t*> param_map_;

    std::auto_ptr<param_set_command_t> command_;
};

} // namespace

#endif
