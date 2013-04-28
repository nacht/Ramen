// Code based in OpenEXR CTL package
// original license:

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Academy of Motion Picture Arts and Sciences
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
//
// A world-wide, royalty-free, non-exclusive right to distribute, copy,
// modify, create derivatives, and use, in source and binary forms, is
// hereby granted, subject to acceptance of this license. Performance of
// any of the aforementioned acts indicates acceptance to be bound by the
// following terms and conditions:
//
//   * Redistributions of source code, in whole or in part, must
//     retain the above copyright notice, this list of conditions and
//     the Disclaimer of Warranty.
//
//   * Redistributions in binary form must retain the above copyright
//     notice, this list of conditions and the Disclaimer of Warranty
//     in the documentation and/or other materials provided with the
//     distribution.
//
//   * Nothing in this license shall be deemed to grant any rights to
//     trademarks, copyrights, patents, trade secrets or any other
//     intellectual property of A.M.P.A.S. or any contributors, except
//     as expressly stated herein.  Neither the name of. A.M.P.A.S. nor
//     any other contributors to this software may be used to endorse or
//     promote products derivative or, or based on this software without
//     express prior written permission of A.M.P.A.S. or contributor, as
//     appropriate.
//
//   * This license shall be construed pursuant to the laws of the State
//     of California, and any disputes related thereto shall be subject
//     to the jurisdiction of the courts therein.
//
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
// EVENT SHALL A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// RESITUTIONARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

#include<ramen/imageio/dpx/dpx_reader.hpp>

#include<stdexcept>

#include<fstream>

namespace ramen
{
namespace imageio
{

dpx_reader_t::dpx_reader_t( const boost::filesystem::path& p) : reader_t( p)
{
    std::ifstream ifile( p.external_file_string().c_str(), std::ios_base::binary);

    if( !ifile)
	throw std::runtime_error( "Can't open file");

    read_header( ifile);

    int width  = getU32 (imageInfo_.pixelsPerLine, byteOrder_);
    int height = getU32 (imageInfo_.linesPerImageElement, byteOrder_);

    info_[adobe::name_t( "domain")] = adobe::any_regular_t( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width - 1, height - 1)));
    info_[adobe::name_t( "gamma")] = adobe::any_regular_t( 1.0);
}

void dpx_reader_t::read_header( std::ifstream& ifile) const
{
    bool strict = false;

    if( !ifile.read( (char *) &fileInfo_, sizeof( fileInfo_)))
	throw std::runtime_error( "Can't read header");

    switch( getU32 (fileInfo_.magicNumber, BO_BIG))
    {
      case 0x53445058:
	byteOrder_ = BO_BIG;
	break;

      case 0x58504453:
	byteOrder_ = BO_LITTLE;
	break;

      default:
	throw std::runtime_error( "Can't open file");
    }

    if( !ifile.read( (char *) &imageInfo_, sizeof( imageInfo_)))
	throw std::runtime_error("Cannot read header");

    if (getU16 (imageInfo_.imageOrientation, byteOrder_) != 0)
	throw std::runtime_error( "Cannot read DPX files with image orientation "
	                  "other than left-to-right, top-to-bottom.");

    if (getU16 (imageInfo_.numberOfElements, byteOrder_) != 1)
	throw std::runtime_error( "Cannot read DPX files with "
			  "multiple image elements.");

    if (getU32 (imageInfo_.imageElements[0].dataSign, byteOrder_) != 0)
	throw std::runtime_error( "Cannot read DPX files with signed data.");

    if (imageInfo_.imageElements[0].descriptor != 50)
	throw std::runtime_error( "Cannot read DPX files with data other than RGB.");

    if (strict &&
	imageInfo_.imageElements[0].transferCharacteristic != 1 &&
	imageInfo_.imageElements[0].transferCharacteristic != 3)
	throw std::runtime_error( "Cannot read DPX files with transfer "
	                  "characteristic other than 'printing "
			  "density' or 'logarithmic.'");

    if (strict &&
	imageInfo_.imageElements[0].colorimetricSpecification != 1)
	throw std::runtime_error("Cannot read DPX files with colorimetric "
	                  "specification other than printing density.");

    if (imageInfo_.imageElements[0].bitSize != 10)
	throw std::runtime_error("Cannot read DPX files with bit size "
			  "other than 10.");

    if (getU16 (imageInfo_.imageElements[0].packing, byteOrder_) != 1)
	throw std::runtime_error("Cannot read DPX files with bit packing "
			  "other than \"filled to 32-bit words.\"");

    if (getU16 (imageInfo_.imageElements[0].encoding, byteOrder_) != 0)
	throw std::runtime_error("Cannot read DPX files with encoded data.");
}

void dpx_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    std::ifstream ifile( path_.external_file_string().c_str(), std::ios_base::binary);

    if( !ifile)
	throw std::runtime_error( "Can't open file");

    int pixelOffset = getU32( imageInfo_.imageElements[0].offsetToData, byteOrder_);
    int width  = getU32 (imageInfo_.pixelsPerLine, byteOrder_);
    int stride = width * 4;

    if( !ifile.seekg( pixelOffset + crop.min.y * stride , std::ios_base::beg))
	throw std::runtime_error("Cannot seek pixel data ");

    std::vector<boost::uint8_t> buffer( stride);
    int yy = 0;

    for( int y = crop.min.y; y <= crop.max.y; y += subsample)
    {
	char *p = reinterpret_cast<char *>( &( buffer.front()));

	if( !ifile.read( p, stride))
	    throw std::runtime_error( "Error reading file");

	// copy row here
	image::image_view_t::x_iterator dst_it( view.row_begin( yy));
	image::image_view_t::x_iterator dst_end( view.row_end( yy));

	U32 *q = reinterpret_cast<U32*>( p);

	for( int x = crop.min.x; x <= crop.max.x; x += subsample)
	{
	    unsigned int word = getU32( q[x], byteOrder_);
	    boost::gil::get_color( *dst_it, boost::gil::red_t())    = ((word >> 22) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::green_t())  = ((word >> 12) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::blue_t())   = ((word >>  2) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = 1.0f;
	    ++dst_it;

	    if( dst_it == dst_end)
		break;
	}

        while( dst_it < dst_end)
        {
            U32 *p = reinterpret_cast<U32*>( &( buffer.back()));
	    unsigned int word = getU32( *p, byteOrder_);
	    boost::gil::get_color( *dst_it, boost::gil::red_t())    = ((word >> 22) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::green_t())  = ((word >> 12) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::blue_t())   = ((word >>  2) & 0x3ff) / 1023.0f;
	    boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = 1.0f;
	    ++dst_it;
        }

        // skip scanlines
	if( !ifile.seekg( (subsample - 1) * stride, std::ios_base::cur))
	    throw std::runtime_error("Cannot seek pixel data ");

        ++yy;
    }

    repeat_scanline_until_end( view, yy - 1);
}

} // namespace
} // namespace
