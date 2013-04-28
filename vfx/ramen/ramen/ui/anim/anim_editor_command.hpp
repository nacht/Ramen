// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_EDITOR_COMMAND_HPP
#define	RAMEN_ANIM_EDITOR_COMMAND_HPP

#include<ramen/undo/command.hpp>

#include<vector>

#include<boost/shared_ptr.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/curve.hpp>

namespace ramen
{
namespace undo
{

class anim_editor_command_t : public command_t
{
public:

    anim_editor_command_t();

    bool empty() const { return tracks_.empty();}

    void add_track( anim::track_t *t);

    const std::vector<anim::track_t *>& tracks() const  { return tracks_;}
    std::vector<anim::track_t *>& tracks()              { return tracks_;}

    anim::track_t *track()
    {
	assert( tracks_.size() == 1);
	return tracks_.front();
    }

    virtual void undo();
    virtual void redo();

private:

    void swap_curves();

    node_t *n_;
    boost::shared_ptr<anim::track_t> track_tree_;

    std::vector<anim::curve_t> old_curves_;
    std::vector<anim::track_t *> tracks_;
};

} // namespace
} // namespace

#endif
