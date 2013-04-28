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

#include<ramen/imageio/dpx/dpx_writer.hpp>

#include <string.h>

#include<fstream>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/imageio/dpx/dpxHeader.h>

namespace {

void writeHeader( std::ofstream &out, unsigned int width, unsigned int height)
{
    union
    {
	struct
	{
	    FileInformation		fileInfo;
	    ImageInformation		imageInfo;
	    ImageOrientation		orientation;
	    MotionPictureFilmHeader	mph;
	    TelevisionHeader		tvh;
	};

	unsigned char			padding[8192];
    } header;

    memset (header.padding, 0xff, sizeof (header));

    setU32 (0x53445058, header.fileInfo.magicNumber, BO_BIG);
    setU32 (sizeof (header), header.fileInfo.offsetToImageData, BO_BIG);
    strcpy (header.fileInfo.versionNumber, "V2.0");

    setU32 (sizeof (header) + 4 * width * height,
	    header.fileInfo.fileSize,
	    BO_BIG);

    header.fileInfo.fileName[0] = 0;
    header.fileInfo.creationTime[0] = 0;
    header.fileInfo.creator[0] = 0;
    header.fileInfo.projectName[0] = 0;
    header.fileInfo.copyright[0] = 0;

    setU16 (0, header.imageInfo.imageOrientation, BO_BIG);
    setU16 (1, header.imageInfo.numberOfElements, BO_BIG);
    setU32 (width, header.imageInfo.pixelsPerLine, BO_BIG);
    setU32 (height, header.imageInfo.linesPerImageElement, BO_BIG);

    setU32 (0, header.imageInfo.imageElements[0].dataSign, BO_BIG); // unsigned
    header.imageInfo.imageElements[0].descriptor = 50;	// RGB
    header.imageInfo.imageElements[0].transferCharacteristic = 3; // log
    header.imageInfo.imageElements[0].colorimetricSpecification = 1; // density
    header.imageInfo.imageElements[0].bitSize = 10;
    setU16 (1, header.imageInfo.imageElements[0].packing, BO_BIG); // filled
    setU16 (0, header.imageInfo.imageElements[0].encoding, BO_BIG); // none

    setU32 (sizeof (header),
	    header.imageInfo.imageElements[0].offsetToData,
	    BO_BIG); // none

    for (int i = 0; i < 7; ++i)
	header.imageInfo.imageElements[i].description[0] = 0;

    header.orientation.sourceImageFileName[0] = 0;
    header.orientation.creationTime[0] = 0;
    header.orientation.inputDev[0] = 0;
    header.orientation.inputSerial[0] = 0;

    header.mph.filmManufacturerId[0] = 0;
    header.mph.filmType[0] = 0;
    header.mph.offset[0] = 0;
    header.mph.prefix[0] = 0;
    header.mph.count[0] = 0;
    header.mph.format[0] = 0;

    if (!out.write ((const char *)&header, sizeof (header)))
	throw std::runtime_error( "can't write header");
}

} // namespace

namespace ramen
{
namespace imageio
{

void dpx_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{

    std::ofstream out( p.external_file_string().c_str(), std::ios_base::binary);

    if( !out)
	throw std::runtime_error( "can't open file");

    writeHeader( out, view.width(), view.height());

    std::vector<boost::uint32_t> buffer( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
	image::const_image_view_t::x_iterator src_it( view.row_begin( y));
	boost::uint32_t *dst = &(buffer.front());

	for( int x = 0; x < view.width(); ++x)
	{
	    unsigned int r = ( adobe::clamp( (float) boost::gil::get_color( *src_it, boost::gil::red_t())    , 0.0f, 1.0f) * 1023.0f) + 0.5f;
	    unsigned int g = ( adobe::clamp( (float) boost::gil::get_color( *src_it, boost::gil::green_t())  , 0.0f, 1.0f) * 1023.0f) + 0.5f;
	    unsigned int b = ( adobe::clamp( (float) boost::gil::get_color( *src_it, boost::gil::blue_t())   , 0.0f, 1.0f) * 1023.0f) + 0.5f;
	    unsigned int word = (r << 22) | (g << 12) | (b << 2);
	    setU32( word, (unsigned char *) dst, BO_BIG);
	    ++src_it;
	    ++dst;
	}

	if( !out.write( (const char *) &( buffer.front()), view.width() * 4))
	    throw std::runtime_error( "can't write to file");
    }
}

} // namespace
} // namespace
