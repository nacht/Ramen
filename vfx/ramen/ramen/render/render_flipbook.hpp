// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_FLIPBOOK_HPP
#define RAMEN_RENDER_FLIPBOOK_HPP

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/flipbook/flipbook_fwd.hpp>

namespace ramen
{
namespace render
{

bool render_flipbook( flipbook::flipbook_t *flip, node_t *n, int start, int end, bool mblur, int subsample = 1);

} // namespace
} // namespace

#endif
