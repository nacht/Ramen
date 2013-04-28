// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIMATED_PARAM_COMMAND_HPP
#define	RAMEN_ANIMATED_PARAM_COMMAND_HPP

#include<ramen/params/animated_param_command_fwd.hpp>

#include<ramen/undo/command.hpp>

#include<memory>
#include<vector>

#include<adobe/any_regular.hpp>

#include<ramen/anim/curve.hpp>

namespace ramen
{

class animated_param_t;

class animated_param_command_t : public undo::command_t
{
public:

    animated_param_command_t( animated_param_t *p);

    virtual void undo();
    virtual void redo();

private:

    void swap_curves();

    animated_param_t *p_;

    adobe::any_regular_t previous_value_, new_value_;

    std::vector<anim::curve_t> old_;
    std::vector<anim::curve_t*> new_;
};

class animated_param_reset_command_t : public undo::command_t
{
public:

    animated_param_reset_command_t( animated_param_t *p);

    virtual void undo();
    virtual void redo();
};

} // namespace

#endif
