// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_HOST_HPP
#define RAMEN_OFX_HOST_HPP

#include<loki/Singleton.h>

#include<boost/noncopyable.hpp>

#include"ofxMultiThread.h"
#include"fnOfxExtensions.h"

#include"ofxhPluginCache.h"
#include"ofxhImageEffectAPI.h"

namespace ramen
{
namespace ofx
{

class host_impl : public OFX::Host::ImageEffect::Host, boost::noncopyable
{
public:

    virtual void loadingStatus( const std::string& s);
    virtual bool pluginSupported( OFX::Host::ImageEffect::ImageEffectPlugin *plugin, std::string &reason) const;

    virtual OFX::Host::ImageEffect::Instance* newInstance(void* clientData,
                                                          OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                          OFX::Host::ImageEffect::Descriptor& desc,
                                                          const std::string& context);

    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(OFX::Host::ImageEffect::ImageEffectPlugin* plugin);

    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const OFX::Host::ImageEffect::Descriptor &rootContext,
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug);

    virtual OFX::Host::ImageEffect::Descriptor *makeDescriptor(const std::string &bundlePath,
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plug);

    void *fetchSuite( const char *suiteName, int suiteVersion);

    /// vmessage
    virtual OfxStatus vmessage( const char* type, const char* id, const char* format, va_list args);

private:

    friend struct Loki::CreateUsingNew<host_impl>;

    host_impl();

    OfxMultiThreadSuiteV1 threading_suite_;
    FnOfxImageEffectPlaneSuiteV1 planev1_suite_;
    FnOfxImageEffectPlaneSuiteV2 planev2_suite_;
};

typedef Loki::SingletonHolder<host_impl> host_t;

} // namespace
} // namespace

#endif
