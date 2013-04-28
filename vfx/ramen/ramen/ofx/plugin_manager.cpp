// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/plugin_manager.hpp>

#include<fstream>

#include<ramen/ofx/host.hpp>
#include<ramen/system/system.hpp>

#include<ramen/app/application.hpp>

#include<iostream>

namespace ramen
{
namespace ofx
{

plugin_manager_t::plugin_manager_t()
{
    OFX::Host::PluginCache::getPluginCache()->setCacheVersion( "RamenV1");
    imageEffectPluginCache_ = new OFX::Host::ImageEffect::PluginCache( ofx::host_t::Instance());

    imageEffectPluginCache_->registerInCache( *OFX::Host::PluginCache::getPluginCache());

    // try to read an old cache
    std::ifstream ifs( cache_path().external_file_string().c_str());
    OFX::Host::PluginCache::getPluginCache()->readCache( ifs);
    OFX::Host::PluginCache::getPluginCache()->scanPluginFiles();
    ifs.close();

    // make our internal list
    std::map<std::string, OFX::Host::ImageEffect::ImageEffectPlugin *>::const_iterator it ( imageEffectPluginCache_->getPluginsByID().begin());
    std::map<std::string, OFX::Host::ImageEffect::ImageEffectPlugin *>::const_iterator end( imageEffectPluginCache_->getPluginsByID().end());

    for( ; it != end; ++it)
    {
	plugin_info_t info;
	info.group  = it->second->getDescriptor().getPluginGrouping();
	info.label  = it->second->getDescriptor().getLabel();
	info.id	    = it->first;

	if( !info.label.empty() && !info.group.empty())
	    plugins_info_.push_back( info);
    }

    // save the plugin cache
    std::ofstream of( cache_path().external_file_string().c_str());
    OFX::Host::PluginCache::getPluginCache()->writePluginCache( of);
    of.close();
}

plugin_manager_t::~plugin_manager_t()
{
    OFX::Host::PluginCache::clearPluginCache();
    delete imageEffectPluginCache_;
}

boost::filesystem::path plugin_manager_t::cache_path() const
{
    return system::system_t::Instance().preferences_path() / "RamenPluginCache.xml";
}

std::auto_ptr<node_t> plugin_manager_t::create_node_by_id( std::string id)
{
    OFX::Host::ImageEffect::ImageEffectPlugin* plugin = imageEffectPluginCache_->getPluginById( id);

    if( plugin)
    {
	try
	{
	    std::auto_ptr<node_t> n( new ofx_node_t( plugin));
	    return n;
	}
	catch( ...)
	{
	    // we let each OFX plugin show it's own error
	}
    }

    return std::auto_ptr<node_t>();
}

std::auto_ptr<node_t> plugin_manager_t::create_node_by_label( std::string label)
{
    OFX::Host::ImageEffect::ImageEffectPlugin* plugin = imageEffectPluginCache_->getPluginByLabel( label);

    if( plugin)
    {
	std::auto_ptr<node_t> n( new ofx_node_t( plugin));
	return n;
    }

    return std::auto_ptr<node_t>();
}

OFX::Host::ImageEffect::ImageEffectPlugin *plugin_manager_t::get_plugin_by_id( const std::string& id, int major, int minor)
{
    return imageEffectPluginCache_->getPluginById( id, major, minor);
}

} // namespace
} // namespace
