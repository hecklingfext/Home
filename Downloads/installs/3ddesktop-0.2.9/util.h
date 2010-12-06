//  ----------------------------------------------------------
//
//  Copyright (C) 2002 Brad Wasson <bard@systemtoolbox.com>
//
//  This file is part of 3ddesktop.
//
//  3ddesktop is free software; you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2, or (at your option)
//  any later version.
//
//  3ddesktop is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with 3ddesktop; see the file COPYING.   If not, write to
//  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
#ifndef _UTIL_H
#define _UTIL_H
#include <math.h>     // for cos
#include <sys/param.h> // BSD 
#ifdef BSD
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#include <X11/Xlib.h>
#include <Imlib2.h>   // for Imlib_Image

#include "3ddesk.h"

#define MAX_FACES  30  // this is a lot

typedef struct polypoints {
    float x[MAX_FACES];
    float y[MAX_FACES];
} polypoints_t;

typedef float matrix_t[4][4];
typedef float vertex_t[4];

// prototypes
void convert_imlib_image_to_opengl_data(int texture_size, Imlib_Image imlib_img, unsigned char *out_buff);
int load_image(char *filename, unsigned char *data, 
               unsigned int *sizex, unsigned int *sizey, int add_alpha);
void clamp_degrees (float *value);
double get_randomf (double low, double high);
int get_randomi (int low, int high);
void ourPrintString(void *font, char *str);
void context_switch(void);
long get_milli_time (void);
void sleep_ms (int ms );
int check_if_i_am_running(void);
int flag_that_i_am_running(void);
int flag_that_i_am_NOT_running(void);
void create_working_dir_if_necessary (void);
int daemon_init (void);
int make_pidfile (void);

// used by CylinderFaceSet
void calculate_polypoints (int num, float side_len, polypoints_t *pp) ;

// used by CarouselFaceSet
void translate (vertex_t v, float x, float y, float z);
void rotate_x (vertex_t v, float x);
void rotate_y (vertex_t v, float y);
void rotate_z (vertex_t v, float z);

inline float deg_to_rad(float deg)
{
    return (PI * deg / 180.0);
}
inline float rad_to_deg(float rad)
{
    return (180 * rad / PI);
}

// theta in degrees
inline float polar_to_x (float r, float theta)
{
    return (r * cos (deg_to_rad(theta)));
}

// theta in degrees
inline float polar_to_y (float r, float theta)
{
    return (r * sin (deg_to_rad(theta)));
}

#ifdef __BIG_ENDIAN__
#define bswap_16(x) (((x) & 0x00ff) << 8 | ((x) & 0xff00) >> 8)
#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#else
#define bswap_16(x) (x)
#define bswap_32(x) (x)
#endif

#endif // _UTIL_H
