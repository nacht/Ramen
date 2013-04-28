// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_IMAGE_VIEW_SHADERS_HPP
#define	RAMEN_UI_IMAGE_VIEW_SHADERS_HPP

namespace ramen
{
namespace ui
{
namespace shaders
{

static const char *color_shader_source =
{
"uniform sampler2D tex;"
"uniform float gamma;"
"uniform float exposure;"
"uniform vec4 channel_mask;"
"void main( void)"
"{"
    "vec4 col = texture2D( tex, gl_TexCoord[0].st);"
    "vec4 expv = vec4( exposure, exposure, exposure, 1);"
    "col *= expv;"
    "vec4 gammav = vec4( gamma, gamma, gamma, 1);"
    "col = pow( col, gammav);"
    "gl_FragColor = max( col * channel_mask, vec4( 0, 0, 0, 0));"
"}"
};

static const char *alpha_shader_source =
{
"uniform sampler2D tex;"
"uniform float gamma;"
"void main( void)"
"{"
	"vec4 col = texture2D( tex, gl_TexCoord[0].st);"
	"vec4 gammav = vec4( gamma, gamma, gamma, gamma);"
	"col = pow( col, gammav);"
	"col = max( col, vec4( 0, 0, 0, 0));"
	"col = min( col, vec4( 1, 1, 1, 1));"
	"gl_FragColor = vec4( col.w, col.w, col.w, 1);"
"}"
};

} // namespace
} // namespace
} // namespace

#endif	/* _IMAGE_VIEW_SHADERS_HPP */

