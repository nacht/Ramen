// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_PALETTE_HPP
#define	RAMEN_UI_PALETTE_HPP

#include<map>

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include<OpenEXR/ImathColor.h>

namespace ramen
{
namespace ui
{

class palette_impl
{
public:

    const Imath::Color3c& operator[]( const std::string& k) const;

private:

    palette_impl();
    ~palette_impl();

    friend struct Loki::CreateUsingNew<palette_impl>;

    std::map<std::string, Imath::Color3c> colors_;
};

typedef Loki::SingletonHolder<palette_impl> palette_t;

} // namespace
} // namespace

#endif	/* _PALETTE_HPP */
