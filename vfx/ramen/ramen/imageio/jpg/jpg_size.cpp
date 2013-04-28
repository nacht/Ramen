#include<ramen/imageio/jpg/jpg_size.hpp>

#include <stdio.h>

/*
  Most of the code that follows was adapted from IJG's rdjpgcom.c file.

 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 *
 * This file contains a very simple stand-alone application that displays
 * the text in COM (comment) markers in a JFIF file.
 * This may be useful as an example of the minimum logic needed to parse
 * JPEG markers.
 */

/* Return next input byte, or EOF if no more */
#define NEXTBYTE()  getc(infile)

/*
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker
 * code byte (which is not an FF).  Here are the marker codes of interest
 * in this program.  (See jdmarker.c for a more complete list.)
 */

#define M_SOF0  0xC0    /* Start Of Frame N */
#define M_SOF1  0xC1    /* N indicates which compression process */
#define M_SOF2  0xC2    /* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5    /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8  /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9    /* End Of Image (end of datastream) */
#define M_SOS   0xDA    /* Start Of Scan (begins compressed data) */
#define M_APP0  0xE0    /* Application-specific marker, type N */
#define M_APP12 0xEC    /* (we don't bother to list all 16 APPn's) */
#define M_COM   0xFE    /* COMment */

#ifdef DONT_USE_B_MODE    /* define mode parameters for fopen() */
#define READ_BINARY "r"
#else
#ifdef VMS      /* VMS is very nonstandard */
#define READ_BINARY "rb", "ctx=stm"
#else       /* standard ANSI-compliant case */
#define READ_BINARY "rb"
#endif
#endif

FILE * infile;

bool process_SOFn (int& width, int& height);
bool skip_variable ();
bool read_1_byte (int* res);
bool read_2_bytes (unsigned int* res);
bool first_marker (int* res);
bool next_marker (int* res);

bool getJPEGSize( const char* filename, int& width, int& height )
{
  //open file
  if ((infile = fopen(filename, READ_BINARY)) == NULL)
    return false;

  //this is scan_JPEG_header (int verbose)
  //Parse the marker stream until SOFn is seen;
  int marker;

  //Expect SOI at start of file
  if (!first_marker(&marker))
  {
    fclose(infile);
    return false;
  }

    /* Scan miscellaneous markers until we reach SOFn. */
  for (;;)
  {
    if(!next_marker(&marker))
    {
      fclose(infile);
      return false;
    }

    switch (marker)
    {
      /* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
       * treated as SOFn.  C4 in particular is actually DHT.
       */
    case M_SOF0:    /* Baseline */
    case M_SOF1:    /* Extended sequential, Huffman */
    case M_SOF2:    /* Progressive, Huffman */
    case M_SOF3:    /* Lossless, Huffman */
    case M_SOF5:    /* Differential sequential, Huffman */
    case M_SOF6:    /* Differential progressive, Huffman */
    case M_SOF7:    /* Differential lossless, Huffman */
    case M_SOF9:    /* Extended sequential, arithmetic */
    case M_SOF10:   /* Progressive, arithmetic */
    case M_SOF11:   /* Lossless, arithmetic */
    case M_SOF13:   /* Differential sequential, arithmetic */
    case M_SOF14:   /* Differential progressive, arithmetic */
    case M_SOF15:   /* Differential lossless, arithmetic */
      if(!process_SOFn(width, height))
      {
        fclose(infile);
        return false;
      }
      else
      {
        fclose(infile);
        return true;
      }
    case M_SOS:     /* stop before hitting compressed data */
    {
      fclose(infile);
      return false;
    }
    case M_EOI:     /* in case it's a tables-only JPEG stream */
    {
      fclose(infile);
      return false;
    }
    default:      /* Anything else just gets skipped */
      skip_variable();    /* we assume it has a parameter count... */
      break;
    }
  } /* end loop */


//cout << "ERROR!\n";
return false;

}


/*
 * Read the initial marker, which should be SOI.
 * For a JFIF file, the first two bytes of the file should be literally
 * 0xFF M_SOI.  To be more general, we could use next_marker, but if the
 * input file weren't actually JPEG at all, next_marker might read the whole
 * file and then return a misleading error message...
 */
bool first_marker (int* res)
{
  int c1, c2;
  c1 = NEXTBYTE();
  c2 = NEXTBYTE();
  if (c1 != 0xFF || c2 != M_SOI)
    return false;
  else
  {
    *res = c2;
    return true;
  }
}

/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 * NB: this routine must not be used after seeing SOS marker, since it will
 * not deal correctly with FF/00 sequences in the compressed image data...
 */
bool next_marker (int* res)
{
  int c;
  int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
  if(!read_1_byte(&c))
    return false;
  while (c != 0xFF)
  {
    discarded_bytes++;
    if(!read_1_byte(&c))
      return false;
  }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
  do
  {
    if(!read_1_byte(&c))
      return false;
  } while (c == 0xFF);

//  if (discarded_bytes != 0) { cout << "Warning: garbage data found in JPEG file\n"; }

  *res = c;
  return true;
}

/* Read one byte, testing for EOF */
bool read_1_byte (int* res)
{
  int c = NEXTBYTE();
  if (c == EOF)
    return false;
  else
  {
    *res = c;
    return true;
  }
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
bool read_2_bytes (unsigned int* res)
{
  int c1, c2;
  c1 = NEXTBYTE();
  if (c1 == EOF)
    return false;
  c2 = NEXTBYTE();
  if (c2 == EOF)
    return false;
  *res = (((unsigned int) c1) << 8) + ((unsigned int) c2);
  return true;
}

/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */
bool skip_variable ()
/* Skip over an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  if(!read_2_bytes(&length))
    return false;
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    return false;
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0)
  {
    int tmp;
    if(!read_1_byte(&tmp))
      return false;
    length--;
  }
  return false;
}

//Process a SOFn marker - get image dimensions
bool process_SOFn (int& width, int& height)
{
  unsigned int length;
  unsigned int image_height, image_width;
  int data_precision;

  if(!read_2_bytes(&length) ||
      !read_1_byte(&data_precision) ||
      !read_2_bytes(&image_height) ||
      !read_2_bytes(&image_width) )
      return false;

  width = (int) image_width;
  height = (int) image_height;
  return true;
}

