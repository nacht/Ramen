// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/ofx_node.hpp>

#include<set>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/serialization/version.hpp>

#include<ramen/ofx/plugin_manager.hpp>
#include<ramen/ofx/host.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/clip.hpp>
#include<ramen/ofx/param.hpp>
#include<ramen/ofx/util.hpp>
#include<ramen/ofx/ofx_manipulator.hpp>

#include<ramen/params/composite_param.hpp>
#include<ramen/params/group_param.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/image_view.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

ofx_node_t::ofx_node_t() : image_node_t(), id_( "null"), effect_( 0), plugin_( 0), descriptor_( 0)
{ 
    set_name( "ofx");
    major_version_ = minor_version_ = 0;
    param_changed_connection_ = param_set().param_changed.connect( boost::bind( &ofx_node_t::param_changed, this, _1));
}

ofx_node_t::ofx_node_t( OFX::Host::ImageEffect::ImageEffectPlugin *plugin) : image_node_t(), effect_( 0), descriptor_( 0)
{
    set_name( plugin->getDescriptor().getLabel());
    id_ = plugin->getIdentifier();
    major_version_ = plugin->getVersionMajor();
    minor_version_ = plugin->getVersionMinor();

    plugin_ = plugin;

    get_descriptor();

    if( !descriptor_)
	throw std::runtime_error( "Can't get descriptor for OFX plugin");

    // clips
    int clip_index = 0;

    BOOST_FOREACH( OFX::Host::ImageEffect::ClipDescriptor *clip, descriptor_->getClipsByOrder())
    {
	ofx::clip_info_t info;

	info.name = clip->getName();
	info.is_optional = clip->isOptional();

	if( clip->isOutput())
	{
	    info.is_output = true;
	    info.index = -1;
	}
	else
	{
	    info.is_output = false;
	    info.index = clip_index++;
	    info.tooltip = clip->getLongLabel();
	}

	clips_info_.push_back( info);
    }

    create_plugs();
    param_changed_connection_ = param_set().param_changed.connect( boost::bind( &ofx_node_t::param_changed, this, _1));
}

ofx_node_t::ofx_node_t( const ofx_node_t& other) : image_node_t( other), id_(other.id_), clips_info_( other.clips_info_)
{
    major_version_ = other.major_version_;
    minor_version_ = other.minor_version_;
    plugin_ = other.plugin_;
    descriptor_ = other.descriptor_;
    context_ = other.context_;
    ofx_context_ = other.ofx_context_;
    effect_ = 0;
    param_changed_connection_ = param_set().param_changed.connect( boost::bind( &ofx_node_t::param_changed, this, _1));
}

ofx_node_t::~ofx_node_t()
{
    // this is to avoid a crash when the node is deleted.
    // It's a hack. Look for the real problem instead...
    manipulators().clear();

    delete effect_;
}

node_t *ofx_node_t::do_clone() const
{
    if( effect_)
    {
	ofx::image_effect_t *fx = const_cast<ofx::image_effect_t*>( effect_);
	fx->syncPrivateDataAction();
    }

    ofx_node_t *new_node = new ofx_node_t( *this);
    new_node->create_effect_instance( false);
    return new_node;
}

void ofx_node_t::get_descriptor()
{
    assert( plugin_ != 0);
    assert( descriptor_ == 0);

    // get the contexts supported by the plugin
    const std::set<std::string>& contexts( plugin_->getContexts());

    if( !ofx_context_.empty())
    {
	// we already have a context, the node was read from a file
	descriptor_ = plugin_->getContext( ofx_context_);
	return;
    }
    
    // try to get a descriptor
    descriptor_ = plugin_->getContext( kOfxImageEffectContextGeneral);

    if( descriptor_)
	ofx_context_ = kOfxImageEffectContextGeneral;
    else
    {
	descriptor_ = plugin_->getContext( kOfxImageEffectContextFilter);

	if( descriptor_)
	    ofx_context_ = kOfxImageEffectContextFilter;
	else
	{
	    descriptor_ = plugin_->getContext( kOfxImageEffectContextGenerator);

	    if( descriptor_)
		ofx_context_ = kOfxImageEffectContextGenerator;
	    else
	    {
		descriptor_ = plugin_->getContext( kOfxImageEffectContextTransition);

		if( descriptor_)
		    ofx_context_ = kOfxImageEffectContextTransition;
	    }
	}
    }
}

void ofx_node_t::create_plugs()
{
    all_inputs_optional_ = true;

    BOOST_FOREACH( ofx::clip_info_t& info, clips_info_)
    {
	if( info.is_output)
	    add_output_plug();
	else
	{
	    input_plug_info_t input_info;

	    if( info.is_optional)
		input_info.set_color( Imath::Color3c( 0, 255, 0));
	    else
		all_inputs_optional_ = false;

	    input_info.set_tooltip( info.tooltip);
	    
	    add_input_plug( input_info, info.is_optional);
	}
    }
}

void ofx_node_t::create_effect_instance( bool call_instance_edit)
{
    assert( effect_ == 0 && "OFX: trying to create an effect instance for a node which already has one");

    // if the plugin is missing
    if( !plugin_)
    {
        set_plugin_error( true);
        return;
    }

    if( !descriptor_)
	get_descriptor();

    std::cout << "OFX: Creating effect instance for plugin " << id_ << "\n";

    // create the effect in the same context it was described in.
    effect_ = dynamic_cast<ofx::image_effect_t*>( plugin_->createInstance( ofx_context_, this));

    if( effect_)
    {
	assert( ofx_context_ == effect_->getContext());

	OfxStatus stat = effect_->createInstanceAction();

	if( stat == kOfxStatOK || stat == kOfxStatReplyDefault)
	    stat = clip_preferences_action();

        if( call_instance_edit)
            stat = effect_->beginInstanceEditAction();
    }
    else
	set_plugin_error( true);
}

int ofx_node_t::index_for_clip_name( const std::string& name) const
{
    BOOST_FOREACH( const ofx::clip_info_t& info, clips_info_)
    {
	if( info.name == name)
	    return info.index;
    }

    // This is for some plugins that try to access clips before they are created.
    if( name == "Source")
	return 0;

    if( name == "Output")
	return -1;

    assert( 0 && "OFX index_for_clip: clip not found");
}

OfxStatus ofx_node_t::clip_preferences_action()
{
    if( is_valid())
    {
	if( effect_->getClipPreferences())
	    return kOfxStatOK;
    }

    return kOfxStatReplyDefault;
}

void ofx_node_t::do_connected( node_t *src, int port)
{
    if( effect_)
    {
	effect_->beginInstanceChangedAction( kOfxChangeUserEdited);
	OfxPointD renderscale;
	float time;

	if( composition())
	{
	    renderscale.x = 1.0 / composition()->subsample();
	    renderscale.y = 1.0 / composition()->subsample();
	    time = composition()->frame();
	}
	else
	{
	    renderscale.x = renderscale.y = 1.0;
	    time = 1.0;
	}

	for( int i = 0; i < effect_->getNClips(); ++i)
	{
	    ofx::clip_t *clip = dynamic_cast<ofx::clip_t*>( effect_->getNthClip( i));

	    if( clip->port() == port)
		effect_->clipInstanceChangedAction( clip->getName(), kOfxChangeUserEdited, time, renderscale);
	}

	effect_->endInstanceChangedAction( kOfxChangeUserEdited);
	clip_preferences_action();
    }
}

void ofx_node_t::do_create_params()
{
    // create the params
    if( descriptor_)
    {
	BOOST_FOREACH( OFX::Host::Param::Descriptor *d, descriptor_->getParamList())
	{
	    std::auto_ptr<param_t> p( ofx::param_factory_t::create_param( d));

	    if( p.get())
	    {
		if( d->getType() == kOfxParamTypePage)
		    add_param( p);
		else
		{
		    // insert the param where it belong
		    if( d->getParentName() != "")
		    {
			composite_param_t *group = find_param( d->getParentName());
			assert( group);
			group->add_param( p);
		    }
		    else
			add_param( p);
		}
	    }
	}
    }

    create_effect_instance();
}

void ofx_node_t::block_param_changed_signal( bool b)
{
    if( b)
	param_changed_connection_.block();
    else
	param_changed_connection_.unblock();
}

void ofx_node_t::param_changed( param_t *p)
{
   if( effect_)
   {
	OfxPointD renderscale;
	renderscale.x = renderscale.y = 1.0;
	float frame = 1.0f;

	if( composition())
	{
	    renderscale.x = 1.0 / composition()->subsample();
	    renderscale.y = 1.0 / composition()->subsample();
	    frame = composition()->frame();
	}

	effect_->beginInstanceChangedAction( kOfxChangeUserEdited);
	effect_->paramInstanceChangedAction( p->id(), kOfxChangeUserEdited, frame, renderscale);
	effect_->endInstanceChangedAction( kOfxChangeUserEdited);

	if( effect_->isClipPreferencesSlaveParam( p->id()))
	    clip_preferences_action();
   }
}

composite_param_t *ofx_node_t::find_param( const std::string& name)
{
    try
    {
	return dynamic_cast<composite_param_t*>( &( param_set().find( name)));
    }
    catch( std::runtime_error& e)
    {
	return 0;
    }
}

void ofx_node_t::do_create_manipulators()
{
    // TODO: implement this
    if( effect_ && effect_->getOverlayInteractMainEntry())
    {
        try
        {
            std::auto_ptr<ofx::ofx_manipulator_t> m( new ofx::ofx_manipulator_t( this, effect()));
            add_manipulator( m);
        }
        catch( ...)
        {
        }
    }
}

const ofx::ofx_manipulator_t *ofx_node_t::overlay() const
{
    if( manipulators().empty())
        return 0;

    const ofx::ofx_manipulator_t *manip = dynamic_cast<const ofx::ofx_manipulator_t *>( &(manipulators()[0]));
    return manip;
}

Imath::Box2i ofx_node_t::vertical_flip_box( const Imath::Box2i& b) const
{
    float cy = ( domain().max.y + domain().min.y) / 2.0f;
    return Imath::Box2i( Imath::V2i( b.min.x, -( b.max.y - cy) + cy),
                         Imath::V2i( b.max.x, -( b.min.y - cy) + cy));
}

Imath::V2f ofx_node_t::vertical_flip_point( const Imath::V2f& p) const
{
    float y0 = domain().min.y * composition()->subsample();
    float y1 = domain().max.y * composition()->subsample();
    float cy = ( y0 + y1) / 2.0f;
    return Imath::V2f( p.x, -( p.y - cy) + cy);
}

bool ofx_node_t::do_is_valid() const { return effect_ != 0;}

bool ofx_node_t::is_frame_varying() const
{
    if( effect()->isFrameVarying() || effect()->temporalAccess())
        return true;
    
    return false;
}

void ofx_node_t::do_calc_hash( const render::render_context_t& context)
{
    append_to_hash( id_);
    std::string t = boost::lexical_cast<std::string>( major_version_);
    append_to_hash( t);

    /* TODO: if the plugin need temporal access,
    // compute a correct hash based in all the frames it needs
    // and don't return true to is_frame_varying ( the current workaround)

    if( effect()->temporalAccess())
    {
        std::string new_hash_str;

        OFX::Host::ImageEffect::RangeMap ranges;
        effect()->getFrameNeededAction( context.time, ranges);
        
        save the current context

        for( each clip instance in ranges)
        {
            for( each range in v)
            {
                for( each time in r)
                // if needed, calc the hash of the input clip at time
            }
        }

        restore the current context
        replace the hash by new hash str here
    }
    */
}

void ofx_node_t::do_calc_bounds( const render::render_context_t& context)
{
    OfxPointD scale;
    scale.x = scale.y = 1.0 / context.subsample;

    OfxRectD area;
    effect_->getRegionOfDefinitionAction( context.time, scale, area);

    Imath::Box2i box( Imath::V2i( area.x1, area.y1), Imath::V2i( area.x2 - 1, area.y2 - 1));

    // flip the rod vertically. OFX coordinate system is y up
    box = vertical_flip_box( box);
    set_bounds( box);
}

void ofx_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    OfxPointD scale;
    scale.x = scale.y = 1.0 / context.subsample;
    OfxRectD area;

    Imath::Box2i roi( interest());

    // flip the roi vertically. OFX coordinate system is y up
    roi = vertical_flip_box( roi);
    ++roi.max.x;
    ++roi.max.y;

    area.x1 = roi.min.x;
    area.y1 = roi.min.y;
    area.x2 = roi.max.x;
    area.y2 = roi.max.y;

    std::map<OFX::Host::ImageEffect::ClipInstance*, OfxRectD> rois;
    effect_->getRegionOfInterestAction( context.time, scale, area, rois);

    assert( rois.size() == num_inputs());

    for( std::map<OFX::Host::ImageEffect::ClipInstance*, OfxRectD>::const_iterator it( rois.begin()); it != rois.end(); ++it)
    {
	ofx::clip_t *clip = dynamic_cast<ofx::clip_t*>( it->first);
	Imath::Box2i box( Imath::V2i( it->second.x1, it->second.y1), Imath::V2i( it->second.x2 - 1, it->second.y2 - 1));

	// flip again as Ramen coordinate system is y down
	box = vertical_flip_box( box);

	if( input( clip->port()))
	    input( clip->port())->add_interest( box);
    }
}

void ofx_node_t::do_calc_defined( const render::render_context_t& context)
{
    if( effect_->supportsTiles())
	set_defined( intersect( bounds(), interest()));
    else
	set_defined( bounds());
}

void ofx_node_t::do_recursive_process( const render::render_context_t& context)
{
    // save the rendering context for later use
    context_ = context;
    adobe::md5_t::digest_t hash_key = digest();

    alloc_image();
    boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 0));

    // we render the input nodes on demand

    process( context);

    assert( hash_key == digest() && "ofx_node_t: digest changed inside process");
}

void ofx_node_t::do_process( const render::render_context_t& context)
{
    OfxPointD scale;
    scale.x = scale.y = 1.0 / context.subsample;

    effect_->beginRenderAction( context.time, context.time, 1, context.mode == render::interface_render, scale);

    Imath::Box2i area;

    if( effect_->supportsTiles())
    {
        area = intersect( defined(), interest());
        area = vertical_flip_box( area);
    }
    else
        area = vertical_flip_box( defined());

    OfxRectI render_window;
    render_window.x1 = area.min.x;
    render_window.y1 = area.min.y;
    render_window.x2 = area.max.x + 1;
    render_window.y2 = area.max.y + 1;

    effect_->renderAction( context.time, kOfxImageFieldBoth, render_window, scale);

    effect_->endRenderAction( context.time, context.time, 1, context.mode == render::interface_render, scale);
    context_.composition = 0;
}

void ofx_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);

    ar & boost::serialization::make_nvp( "id", id_);
    ar & boost::serialization::make_nvp( "major_version", major_version_);
    ar & boost::serialization::make_nvp( "minor_version", minor_version_);

    ar & boost::serialization::make_nvp( "ofx_clips_info", clips_info_);
    create_plugs();

    plugin_ = application_t::Instance().ofx_plugin_manager().get_plugin_by_id( id_, major_version_, minor_version_);
    ar & boost::serialization::make_nvp( "ofx_context", ofx_context_);
}

void ofx_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    if( effect_)
    {
	ofx::image_effect_t *fx = const_cast<ofx::image_effect_t*>( effect_);
	fx->syncPrivateDataAction();
    }

    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);

    ar & boost::serialization::make_nvp( "id", id_);
    ar & boost::serialization::make_nvp( "major_version", major_version_);
    ar & boost::serialization::make_nvp( "minor_version", minor_version_);

    ar & boost::serialization::make_nvp( "ofx_clips_info", clips_info_);
    ar & boost::serialization::make_nvp( "ofx_context", ofx_context_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::ofx_node_t)
