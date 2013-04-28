// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMPOSITE_PARAM_HPP
#define	RAMEN_COMPOSITE_PARAM_HPP

#include<ramen/params/param.hpp>

#include<memory>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/undo/command.hpp>

namespace ramen
{

class composite_param_t : public param_t
{
    Q_OBJECT

public:

    composite_param_t();
    explicit composite_param_t( const std::string& name);

    // this is for OFX
    virtual void copy( const param_t& other, float offset = 0.0f);
    virtual void copy( const param_t& other, float offset, float start, float end);

    virtual void set_param_set( param_set_t *parent);

    const boost::ptr_vector<param_t>& params() const	{ return params_;}
    boost::ptr_vector<param_t>& params()		{ return params_;}

    template<class T>
    void add_param( std::auto_ptr<T> p) { do_add_param( p.release());}

    virtual std::auto_ptr<undo::command_t> create_reset_command();

protected:

    composite_param_t( const composite_param_t& other);
    void operator=( const composite_param_t& other);

    void create_widgets_inside_widget( QWidget *parent);

private:

    virtual param_t *do_clone() const { return new composite_param_t( *this);}

    virtual void do_init();

    void do_add_param( param_t *p);

    virtual void do_evaluate( float time);
    virtual std::string do_str() const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    virtual void do_create_tracks( anim::track_t *parent);

    virtual void do_domain_changed( const Imath::Box2i& new_domain);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    boost::ptr_vector<param_t> params_;
};

} // namespace

#endif
