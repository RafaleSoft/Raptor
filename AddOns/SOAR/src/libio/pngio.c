/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <png.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include "common.h"
#include "pngio.h"

/*@p-r-i-v-a-t-e---d-a-t-a---------------------------------------------------*/

static const union {
  png_uint_16   u;
  struct {
    unsigned char       little;
    unsigned char       big;
  }             is;
} endian = { 1 };

/*@p-u-b-l-i-c---f-u-n-c-t-i-o-n-s-------------------------------------------*/

/*@png_read..................................................................*/

void *
png_read(
  unsigned      *size_x,        /* image width */
  unsigned      *size_y,        /* image height */
  unsigned      *size_z,        /* number of channels */
  unsigned      *depth,         /* datum depth in bytes */
  const char    *filename,      /* path */
  BOOL          byteize         /* convert to 8-bit depth when set */
)
{
  void          *image;
  FILE          *fp;
  png_bytep     *row;
  png_byte      magic[8];
  png_structp   png;
  png_infop     info;
  unsigned      i;

  /*
  ** Wrapper for reading a PNG file.
  */

  assert(!endian.is.little == !!endian.is.big);

  if (!(fp = fopen(filename, "rb"))) {
    fprintf(stderr, "can't open '%s': %s\n", filename, strerror(errno));
    return NULL;
  }
  if (fread(magic, sizeof(magic), 1, fp) != 1) {
    fprintf(stderr, "can't read '%s': %s\n", filename, strerror(errno));
    fclose(fp);
    return NULL;
  }
  if (!png_check_sig(magic, sizeof(magic))) {
    fprintf(stderr, "'%s' is not a PNG file\n", filename);
    fclose(fp);
    return NULL;
  }

  if (!(png =
          png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
    fprintf(stderr, "PNG read struct allocation failed\n");
    fclose(fp);
    return NULL;
  }

  if (!(info = png_create_info_struct(png))) {
    fprintf(stderr, "PNG info struct allocation failed\n");
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    return NULL;
  }

  if (setjmp(png_jmpbuf(png))) {
    fprintf(stderr, "setjmp() failed\n");
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return NULL;
  }

  png_init_io(png, fp);
  png_set_sig_bytes(png, sizeof(magic));
  png_read_info(png, info);
  png_set_packing(png);
  if (byteize)
    png_set_strip_16(png);
  else if (endian.is.little)
    png_set_swap(png);
  png_read_update_info(png, info);

  assert(info->bit_depth % CHAR_BIT == 0);
  *size_x = (unsigned)info->width;
  *size_y = (unsigned)info->height;
  *size_z = (unsigned)info->channels;
  if (depth)
    *depth = (unsigned)info->bit_depth / CHAR_BIT;

  if (!(row = malloc(*size_y * sizeof(png_bytep))) ||
      !(image = row[*size_y - 1] = malloc(*size_y * info->rowbytes))) {
    fprintf(stderr, "malloc() for PNG data failed\n");
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return NULL;
  }
  for (i = *size_y - 1; i; i--)
    row[i - 1] = row[i] + info->rowbytes;

  png_read_image(png, row);
  png_read_end(png, NULL);
  png_destroy_read_struct(&png, &info, NULL);

  free(row);
  fclose(fp);

  return image;
}
