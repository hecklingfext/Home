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
#ifndef _3DDESK_H
#define _3DDESK_H

#include "config.h"

// define to just use carousel faceset
//#define ONE_ARRANGEMENT_ONLY  1  // for debugging


#define PI         3.141592654
#define FOV        60.0   // field of view

#define MAX_FILTER GL_LINEAR // scale linearly when image bigger than texture
#define MIN_FILTER GL_LINEAR_MIPMAP_LINEAR


#define MTYPE_ACTIVATE          1
#define MTYPE_ACQUIRE           2
#define MTYPE_ACQUIRE_CURRENT   3
#define MTYPE_STOP              4
#define MTYPE_JUSTSWITCH        5


// positive - goto that face
// 0 -- no goto
// -1 -- goto right
// -2 -- goto left
// -3 -- goto up
// -4 -- goto down
#define NO_GOTO          0
#define GOTO_FACE_RIGHT -1
#define GOTO_FACE_LEFT  -2
#define GOTO_FACE_UP    -3
#define GOTO_FACE_DOWN  -4


// these are indexes into an array so be careful!
enum FaceType_t {
    FACE_TYPE_CAROUSEL = 0,
    FACE_TYPE_CYLINDER,
    FACE_TYPE_LINEAR,
    FACE_TYPE_VIEWMASTER,
    FACE_TYPE_PRICEISRIGHT,
    FACE_TYPE_FLIP,
    FACE_TYPE_LAST,

    FACE_TYPE_RANDOM = 888,
    FACE_TYPE_NONE = 999,
};


#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_BLUE      3 
#define COLOR_LIGHTBLUE 4
#define COLOR_WHITE     5
#define COLOR_GRAY      6
#define COLOR_PURPLE    7
#define COLOR_YELLOW    8


#include <dirent.h>  // for PATH_MAX (i think)

typedef struct desktop_coords {
    int column;
    int row;
    // int workspace;
} desktop_coords_t;


#define DEBUG 1
#define INFO  2
#define WARN  3
#define ERROR 4

//#define printf   HEY_DUMMY_DONT_USE_PRINTF_USE_MSGOUT

void msgout (int level, char *msg, ...);
void end_program (int rv);

#endif // _3DDESK_H
