// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_PLUGIN_MANAGER_HPP
#define RAMEN_OFX_PLUGIN_MANAGER_HPP

#include<ramen/ofx/plugin_manager_fwd.hpp>

#include<boost/noncopyable.hpp>
#include<boost/filesystem/path.hpp>

#include"ofxCore.h"

#include"ofxhBinary.h"
#include"ofxhPropertySuite.h"
#include"ofxhClip.h"
#include"ofxhParam.h"
#include"ofxhMemory.h"
#include"ofxhImageEffect.h"
#include"ofxhPluginAPICache.h"
#include"ofxhPluginCache.h"
#include"ofxhHost.h"
#include"ofxhImageEffectAPI.h"

#include<ramen/app/application_fwd.hpp>
#include<ramen/ofx/ofx_node.hpp>

namespace ramen
{
namespace ofx
{

class plugin_manager_t : boost::noncopyable
{
public:

    struct plugin_info_t
    {
	std::string group;
	std::string label;
	std::string id;
    };

    typedef std::vector<plugin_info_t>::const_iterator const_iterator;
    const_iterator begin() const    { return plugins_info_.begin();}
    const_iterator end() const	    { return plugins_info_.end();}

    std::auto_ptr<node_t> create_node_by_id( std::string id);
    std::auto_ptr<node_t> create_node_by_label( std::string label);

    OFX::Host::ImageEffect::ImageEffectPlugin *get_plugin_by_id( const std::string& id, int major = -1, int minor = -1);

private:

    friend class ::ramen::application_impl;

    plugin_manager_t();
    ~plugin_manager_t();

    boost::filesystem::path cache_path() const;

    OFX::Host::ImageEffect::PluginCache *imageEffectPluginCache_;

    std::vector<plugin_info_t> plugins_info_;
};

} // namespace
} // namespace

#endif
