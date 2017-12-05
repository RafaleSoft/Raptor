/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#if WITH_MEMORY_MAP
  #ifdef WIN32
    #include <windows.h>
    #define BOOL_DEFINED
  #else
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
  #endif
#endif

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glut.h>
#include "common.h"
#include "terrain.h"
#include "geoio.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define GEO_HEADER_LENGTH       offsetof(GEOheader, data)
#define GEO_FORMAT              0x4u
#define GEO_MAGIC               ('g' << 0x18 | 'e' << 0x10 | 'o' << 0x08)

/*@p-r-i-v-a-t-e---t-y-p-e-s-------------------------------------------------*/

typedef struct {
  unsigned              magic;          /* magic string */
  unsigned              format;         /* file format identifier */
  unsigned short        levels;         /* number of refinement levels */
  unsigned short        indexing;       /* indexing method */
  unsigned              count;          /* number of vertices including gaps */
  VECTOR3f              min;            /* bounding box minima */
  VECTOR3f              max;            /* bounding box maxima */
  VERTEX                data[1];        /* for proper alignment */
} GEOheader;

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static BOOL
geo_header_compatible(
  const GEOheader       *header,        /* GEO header from file */
  unsigned              indexing        /* indexing scheme */
);

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@geo_header_valid..........................................................*/

static BOOL
geo_header_compatible(
  const GEOheader       *header,        /* GEO header from file */
  unsigned              indexing        /* indexing scheme */
)
{
  if (header->magic != GEO_MAGIC) {
    fprintf(stderr, "file is not a GEO file\n");
    return FALSE;
  }
  if (header->format != GEO_FORMAT) {
    fprintf(stderr, "unknown GEO format\n");
    return FALSE;
  }
  if (header->indexing != indexing) {
    fprintf(stderr, "GEO indexing scheme mismatch\n");
    return FALSE;
  }

  return TRUE;
}

/*@p-u-b-l-i-c---f-u-n-c-t-i-o-n-s-------------------------------------------*/

/*@geo_read..................................................................*/

VERTEX *
geo_read(
  VECTOR3f      box[2],         /* bounding box min, max */
  unsigned      *count,         /* number of vertices including gaps */
  unsigned      *levels,        /* number of refinement levels */
  const char    *filename,      /* path to GEO file */
  unsigned      indexing        /* indexing scheme */
)
{
  FILE          *fp;
  GEOheader     header;
  void          *data;

  if (!(fp = fopen(filename, "rb"))) {
    fprintf(stderr, "can't open '%s': %s\n", filename, strerror(errno));
    return NULL;
  }
  if (fread(&header, GEO_HEADER_LENGTH, 1, fp) != 1) {
    fprintf(stderr, "can't read '%s': %s\n", filename, strerror(errno));
    fclose(fp);
    return NULL;
  }
  if (!geo_header_compatible(&header, indexing)) {
    fclose(fp);
    return NULL;
  }

#if WITH_MEMORY_MAP
  fclose(fp);
  #ifdef WIN32
  /*
  ** Windows option; memory map file.
  */
  {
    HANDLE      file, mapping;

    if (!(file =
            CreateFile(
              filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL
            ))) {
      fprintf(stderr, "can't open '%s'\n", filename);
      return NULL;
    }
    if (!(mapping =
            CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL))) {
      fprintf(stderr, "CreateFileMapping() failed\n");
      CloseHandle(file);
      return NULL;
    }
    if (!(data = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0))) {
      fprintf(stderr, "MapViewOfFile() failed\n");
      CloseHandle(mapping);
      CloseHandle(file);
      return NULL;
    }
  }
  #else
  /*
  ** Unix option; memory map file.
  */
  {
    size_t      len = GEO_HEADER_LENGTH + header.count * sizeof(VERTEX);
    int         fd;

    if ((fd = open(filename, O_RDONLY)) == -1) {
      fprintf(stderr, "can't open '%s': %s\n", filename, strerror(errno));
      return NULL;
    }
    if ((data = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
      fprintf(stderr, "mmap() failed: %s\n", strerror(errno));
      close(fd);
      return NULL;
    }
  }
  #endif
  data = (void *)((char *)data + GEO_HEADER_LENGTH);
#else
  /*
  ** Allocate memory and read entire file.
  */
  if (!(data = malloc(header.count * sizeof(VERTEX)))) {
    fprintf(stderr, "malloc() failed: %s\n", strerror(errno));
    fclose(fp);
    return NULL;
  }
  if (fread(data, sizeof(VERTEX), header.count, fp) != header.count) {
    fprintf(stderr, "can't read '%s': %s\n", filename, strerror(errno));
    free(data);
    fclose(fp);
    return NULL;
  }
  fclose(fp);
#endif

  COPY(box[0], header.min);
  COPY(box[1], header.max);
  *count = header.count;
  *levels = 2 * header.levels;

  return data;
}

/*@geo_write.................................................................*/

BOOL
geo_write(
  const char            *filename,      /* path to GEO file */
  unsigned              indexing,       /* indexing scheme */
  const VERTEX          *vv,            /* 1D array of vertices */
  const VECTOR3f        box[2],         /* bounding box min, max */
  unsigned              count,          /* number of vertices including gaps */
  unsigned              levels          /* number of refinement levels */
)
{
  FILE                  *fp;
  GEOheader             header;

  if (!(fp = fopen(filename, "wb"))) {
    fprintf(stderr, "can't create '%s': %s\n", filename, strerror(errno));
    return FALSE;
  }

  header.magic = GEO_MAGIC;
  header.format = GEO_FORMAT;
  header.levels = levels / 2;
  header.indexing = (unsigned short)indexing;
  header.count = count;
  COPY(header.min, box[0]);
  COPY(header.max, box[1]);

  if (fwrite(&header, GEO_HEADER_LENGTH, 1, fp) != 1 ||
      fwrite(vv, sizeof(VERTEX), count, fp) != count) {
    fprintf(stderr, "can't write '%s': %s\n", filename, strerror(errno));
    fclose(fp);
    return FALSE;
  }
  fclose(fp);

  return TRUE;
}
