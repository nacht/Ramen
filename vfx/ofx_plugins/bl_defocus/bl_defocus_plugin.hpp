/**
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#ifndef OFX_BL_DEFOCUS_PLUGIN_HPP
#define OFX_BL_DEFOCUS_PLUGIN_HPP

#include "ofxsCore.h"
#include "ofxsImageEffect.h"

struct bl_defocus_plugin_factory : public OFX::PluginFactoryHelper<bl_defocus_plugin_factory>
{
    bl_defocus_plugin_factory():OFX::PluginFactoryHelper<bl_defocus_plugin_factory>( "blender.openfx:bl_defocus", 1, 0) {}

    virtual void describe( OFX::ImageEffectDescriptor &desc);

    virtual void describeInContext( OFX::ImageEffectDescriptor &desc, OFX::ContextEnum context);
    virtual OFX::ImageEffect *createInstance( OfxImageEffectHandle handle, OFX::ContextEnum context);
};

#endif
