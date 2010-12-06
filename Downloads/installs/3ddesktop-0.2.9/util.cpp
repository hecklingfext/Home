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
//  ------
//  This file is where I dump all the misc functions I don't know what 
//  else to do with...
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>  // for gettimeofday 
#include <time.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <syslog.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <dirent.h>

//#include <GL/glut.h> // GLUT support library.

#include "util.h"
#include "config.hpp"
#include "message.hpp"

extern Config *cfg;


void msgout (int level, char *msg, ...)
{
    va_list ap;

    // FIXME: change if threads are added 
    static char buff[4096];  
    static FILE *logfp = stdout;

    if (level == DEBUG && !cfg->verbose)
        return;
    
    if (cfg->options->daemonized) {
        // only log to syslog
        va_start (ap, msg);
        vsnprintf (buff, 4096, msg, ap);
        va_end (ap);
        
        syslog (LOG_ERR, buff);
    } else {
        // not daemonized meaning verbose is on (debugging)
        va_start(ap, msg);
        vfprintf (logfp, msg, ap);
        va_end(ap);

        fflush(logfp);
    }
}


void 
convert_imlib_image_to_opengl_data(int texture_size, Imlib_Image imlib_img, unsigned char *out_buff)
{
    imlib_context_set_image(imlib_img);
    Imlib_Image imgfinal = imlib_create_cropped_scaled_image(0, 0, 
                                            imlib_image_get_width(), 
                                            imlib_image_get_height(),
                                            texture_size,
                                            texture_size);  

    imlib_context_set_image(imgfinal);
    unsigned int *tmp = imlib_image_get_data();

    int w, h;
    int x, y;
    int offset;
    int img_offset;

    w = h = texture_size;
    
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            offset = (y * w + x) * 4;
            img_offset = (y * w + x);

            out_buff[offset]     = (tmp[img_offset] >> 16) & 0xff;
            out_buff[offset + 1] = (tmp[img_offset] >> 8) & 0xff;
            out_buff[offset + 2] =  tmp[img_offset] & 0xff;
            out_buff[offset + 3] = (tmp[img_offset] >> 24) & 0xff;
        }
    }
    imlib_image_put_back_data(tmp);
    imlib_free_image();
}


// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.
int load_image(char *filename, 
               unsigned char *data, 
               unsigned int *sizex, 
               unsigned int *sizey,
               int add_alpha) 
{
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    unsigned int j;
    unsigned int byte_multiplier = 3;
    
    unsigned char *working_buff = NULL;

    unsigned int size_x, size_y;

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
        // don't print anything because we may be opening digit files
        // or something that we don't care if they aren't there

	//msgout(ERROR,"Could not open %s: %s\n",filename, strerror(errno));
	return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&size_x, 4, 1, file)) != 1) {
	msgout(ERROR,"Error reading width from %s.\n", filename);
	return 0;
    }

    // expected size is passed it - fail if its not as expected
    if (size_x != *sizex) {
        msgout(DEBUG,"Error: width of %s is %u not %u\n", filename, size_x, *sizex);
        return 0;
    }
    *sizex = bswap_32(size_x);
    
    // read the height 
    if ((i = fread(&size_y, 4, 1, file)) != 1) {
	msgout(ERROR,"Error reading height from %s.\n", filename);
	return 0;
    }

    // expected size is passed in - fail if its not as expected
    if (size_y != *sizey) {
        msgout(DEBUG,"Error: height of %s is %u not %u\n", filename, size_y, *sizey);
        return 0;
    }
    *sizey = bswap_32(size_y);
    
    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	msgout(ERROR,"Error reading planes from %s.\n", filename);
	return 0;
    }
    planes = bswap_16(planes);

    if (planes != 1) {
	msgout(ERROR,"Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	msgout(ERROR,"Error reading bpp from %s.\n", filename);
	return 0;
    }
    bpp = bswap_16(bpp);

    //msgout(DEBUG,"BPP from %s is %u\n", filename, bpp);

    
    // 32 bits per pix is same as 24 (8 bits per color) except the
    // last 8 bits are used for other stuff
    if ( ! (bpp == 24 || bpp == 32)  ) {
        msgout(ERROR,"Bpp from %s is not 24 or 32: %u\n", filename, bpp);
        return 0;
    }

    if (bpp == 24)
        byte_multiplier = 3;
    else if (bpp == 32)
        byte_multiplier = 4;
	
    // calculate the size 
    size = (*sizex) * (*sizey) * byte_multiplier;

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);


    // allocate buff
    working_buff = new unsigned char[(*sizex) * (*sizey) * 4];
    if (working_buff == NULL) {
        msgout(ERROR,"Out of memory\n");
        exit (1);
    }


    if ((i = fread(working_buff, size, 1, file)) != 1) {
	msgout(ERROR,"Error reading image data from %s.\n", filename);
	return 0;
    }

    // if the bpp is 32 we need to "squeeze" it down to 24bpp so j is
    // the index at 24bpp
    j = 0; //(*sizex) * (*sizey) * 3;

    int x,y;

    for (y = (*sizey) - 1; y >= 0; y--) {

        //for (x = (*sizex) - 1; x >= 0; x--) {
        for (x = 0; x < (int)(*sizex); x++) {

            int offset = (y * (*sizey) + x) * byte_multiplier;
            
            //for (i = 0; i < size; i += byte_multiplier) {

            // reverse all of the colors. (bgr -> rgb)
            data[j] = working_buff[offset + 2];
            data[j + 1] = working_buff[offset + 1];
            data[j + 2] = working_buff[offset];

            if (add_alpha)
                if (data[j] == 0 && data[j + 1] == 0 && data[j + 2] == 0)
                    data[j + 3] = 0;  // black make transparent...
                else
                    data[j + 3] = 1;
            
            j += add_alpha ? 4 : 3;
        }
    }

    delete working_buff;

    // we're done.
    return 1;
}


void clamp_degrees (float *value) 
{
    //msgout (DEBUG,"clamp IN %f\n", *value);
    while (*value > 360.0)
        *value -= 360.0;
    while (*value < -360.0)
        *value += 360.0;
    //msgout(DEBUG,"clamp OUT %f\n", *value);
}


double get_randomf (double low, double high)
{
    return (low + ((high - low) * rand() / (RAND_MAX+1.0)));
}

int get_randomi (int low, int high)
{
    return ( (int)((double)low + ((double)(high-low+1) * rand() / (RAND_MAX+1.0))) );
}


void context_switch(void)
{
    struct timeval tm;

    // is there a better way to do this?  
    // sleep(0) doesn't seem to work

    tm.tv_sec = 0;
    tm.tv_usec = 100;

    select (0, 0, 0, 0, &tm); /* sleep */
}

long get_milli_time (void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (long)(tv.tv_usec / 1000.0));
} // END get_milli_time


void sleep_ms (int ms )
{
    struct timeval tm;

    tm.tv_sec = (time_t)floor(ms / 1000);
    tm.tv_usec = (ms - (tm.tv_sec * 1000)) * 1000;

    select (0, 0, 0, 0, &tm); /* sleep */
}

#if 0

// uses a glut routine
void 
ourPrintString(void *font, char *str)
{
    int i,l=strlen(str);

    for(i=0;i<l;i++)
        glutBitmapCharacter(font,*str++);
} // END ourPrintString

#endif // if 0


#ifdef FILE_METHOD
static char running_file[200] = { 0, };
#endif

int check_if_i_am_running(void)
{
    // I like using a semaphore for this purpose because it will
    // automatically be decremented for us when we exit - regardless
    // of how the process was killed plus I have knowledge about this
    // sys V crap ;)

    // btw can't use POSIX sem_init etc here because linux doesn't do
    // inter-process apparently... :(   (whats the use?)

    union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
    } semstat;

    int rc;
    key_t key = 0;
    int semid = 0;

    key = ftok("/bin/sh", 99);
    if (key < 0) {
        msgout(ERROR,"ftok failed: %d: %s\n", errno, strerror(errno));
        return 1;
    }

    semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL );
    if (semid < 0) {
        // if the semaphore exists read it and if its non-zero fail
        if (errno == EEXIST) {
            //msgout (DEBUG,"EXISTS!\n");

            semid = semget(key, 1, 0666);
            if (semid < 0) {
                msgout(ERROR,"semget (2) failed: %d: %s\n",
                        errno, strerror(errno));
                return 1;
            }
    
            if ( (rc = semctl (semid, 0, GETVAL, semstat )) < 0 )
            {
                msgout (ERROR, "semctl GETVAL zero failed: %d: %s\n",
                        errno, strerror(errno));
                return 1;
            }
            //msgout (DEBUG, "sem value == %d\n", rc);
            
            return (rc != 0);

        } else {
            // semget failed for some weird reason
            msgout(ERROR, "semget failed: %d: %s\n", errno, strerror(errno));
            return 1;
        }
    } else {
        //msgout (DEBUG, "CREATED!\n");

        // we must init to zero since there is no gaurantee of its
        // initial value.  this is the fatal flaw of sysV ipc.
        // Stevens provides an algorithm to make initialization
        // safe (just initializing it is not technically safe) but
        // this race is very unlikely (I hope!)
        semstat.val = 0;
        if ( semctl (semid, 0, SETVAL, semstat ) < 0 )
        {
            msgout (ERROR, "semctl SETVAL zero failed: %d: %s\n",
                    errno, strerror(errno));
            return 1;
        }
    }

    return 0;
}


int flag_that_i_am_running(void)
{

    // I like using a semaphore for this purpose because it will
    // automatically be decremented for us when we exit - regardless
    // of how the process was killed plus I have knowledge about this
    // sys V crap ;)

    // btw can't use POSIX sem_init etc here because linux doesn't do
    // inter-process apparently... :(   (whats the use?)

    union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
    } semstat;

    int rc;
    key_t key = 0;
    int semid = 0;
    struct sembuf chain_lock[2];

    key = ftok("/bin/sh", 99);
    if (key < 0) {
        msgout(ERROR,"ftok failed: %d: %s\n", errno, strerror(errno));
        return 1;
    }


    semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL );
    if (semid < 0) {
        // if the semaphore exists read it and if its non-zero fail
        if (errno == EEXIST) {
            //msgout (DEBUG,"EXISTS!\n");

            semid = semget(key, 1, 0666);
            if (semid < 0) {
                msgout(ERROR,"semget (2) failed: %d: %s\n",
                        errno, strerror(errno));
                return 1;
            }
    
        } else {
            // semget failed for some weird reason
            msgout(ERROR, "semget failed: %d: %s\n", errno, strerror(errno));
            return 1;
        }
    } else {
        // this should never happen since we created in
        // check_if_i_am_running() above....

        //msgout (DEBUG, "CREATED!\n");

        // we must init to zero since there is no gaurantee of its
        // initial value.  this is the fatal flaw of sysV ipc.
        // Stevens provides an algorithm to make initialization
        // safe (just initializing it is not technically safe) but
        // this race is very unlikely (I hope!)
        semstat.val = 0;
        if ( semctl (semid, 0, SETVAL, semstat ) < 0 )
        {
            msgout (ERROR, "semctl SETVAL zero failed: %d: %s\n",
                    errno, strerror(errno));
            return 1;
        }
    }
    
#if 0
    if ( (rc = semctl (semid, 0, GETVAL, semstat )) < 0 )
    {
        msgout (ERROR, "semctl GETVAL zero failed: %d: %s\n",
                errno, strerror(errno));
        return 1;
    }
    msgout (DEBUG, "sem value == %d\n", rc);
#endif

    // we either just created the semaphore or we got a handle to the
    // existing one ... init it to 1 atomically and we're golden
        
    // check if its zero and if its zero increment to 1
    // atomically - SEM_UNDO will reset when process dies
    chain_lock[0].sem_num = 0;
    chain_lock[0].sem_op  = 0;
    chain_lock[0].sem_flg = IPC_NOWAIT;
    chain_lock[1].sem_num = 0;
    chain_lock[1].sem_op  = 1;
    chain_lock[1].sem_flg = SEM_UNDO | IPC_NOWAIT;
    
    rc = semop(semid, &chain_lock[0], 2);
    if ( rc < 0 ) {
        if (errno == EAGAIN) {
            // would block meaning the semaphore is non-zero
            // so somebody else has it - return 1 :(
            return 1;
        } else {
            msgout (ERROR, "semop failed: %d: %s\n", errno,
                    strerror(errno));
            return 1;
        }
    }
    
    // we successfully incremented to 1 - we're golden
    return 0;

#ifdef FILE_METHOD

    FILE *fp = NULL;
    struct stat statbuf;

    sprintf(running_file, "%s/running", cfg->options->working_dir);

    if (stat(running_file, &statbuf) < 0) {
        fp = fopen (running_file, "w");
        if (fp == NULL) {
            msgout (ERROR, "Could not create %s: %s\n", running_file, strerror(errno));
            return 1;
        }
        fclose (fp);
    } else {
        // file exists so fail
        return 1;
    }

    return 0;
#endif

}

int flag_that_i_am_NOT_running(void)
{
#ifdef FILE_METHOD
    if (running_file[0] != 0)
        // just delete the file
        unlink (running_file);
#endif

    return 0;
}



//-------------------------------------------------
//  Functions used by faceset.cpp
//-------------------------------------------------



// calculates the points in an n-sided polygon
void calculate_polypoints (int num, float side_len, polypoints_t *pp) 
{
    int i;
    float side_angle = 360.0 / (float)num;

    float r = (side_len/2.0) / sin(deg_to_rad(side_angle/2.0));

    float theta = -side_angle/2.0 - 90.0;  // to start

    //msgout(DEBUG,"r = %lf, theta = %lf\n", r, theta);

    for (i = 0; i < num; i++) {
        pp->x[i] = polar_to_x (r, theta);
        pp->y[i] = polar_to_y (r, theta);

        theta += side_angle;
    }
}



void translate (vertex_t v, float x, float y, float z)
{
    v[0] += x;
    v[1] += y;
    v[2] += z;
}

// multiply m times v and put result in v
static void multiply (matrix_t m, vertex_t v) 
{
    vertex_t r;
    r[0] =  m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
    r[1] =  m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
    r[2] =  m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
    r[3] =  m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3];
    v[0] = r[0];
    v[1] = r[1];
    v[2] = r[2];
    v[3] = r[3];
}


static void load_identity (matrix_t m)
{
    memset(m, 0, sizeof(matrix_t));
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
}


//      Rotation

//         | 1      0        0   0 |
// Rx(A) = | 0  cos A   -sin A   0 |
//         | 0  sin A    cos A   0 |
//         | 0      0        0   1 |
 
//         | cos A   0   sin A   0 |
// Ry(A) = |     0   1       0   0 |
//         | -sin A  0   cos A   0 |
//         |     0   0       0   1 |

//         | cos A  -sin A   0   0 |
// Rz(A) = | sin A   cos A   0   0 |
//         |     0       0   1   0 |
//         |     0       0   0   1 |

void rotate_x (vertex_t v, float x)
{
    float xr = deg_to_rad(x);
    float tmp;
    matrix_t m;

    // x
    load_identity (m);
    tmp = cos(xr);
    m[1][1] = tmp;
    m[2][2] = tmp;
    tmp = sin(xr);
    m[1][2] = -tmp;
    m[2][1] = tmp;

    multiply (m, v);
}
    
void rotate_y (vertex_t v, float y)
{
    float yr = deg_to_rad(y);
    float tmp;
    matrix_t m;

    // y
    load_identity (m);
    tmp = cos(yr);
    m[0][0] = tmp;
    m[2][2] = tmp;
    tmp = sin(yr);
    m[0][2] = tmp;
    m[2][0] = -tmp;


    multiply(m,v);
}
void rotate_z (vertex_t v, float z)
{
    float zr = deg_to_rad(z);
    float tmp;
    matrix_t m;

    // z
    load_identity (m);
    tmp = cos(zr);
    m[0][0] = tmp;
    m[1][1] = tmp;
    tmp = sin(zr);
    m[0][1] = -tmp;
    m[1][0] = tmp;

    multiply(m,v);
}



int daemon_init (void)
{
    pid_t pid;

    msgout (DEBUG, "daemon init\n");
    
    if ( (pid = fork()) < 0) {
        msgout (ERROR, "daemon_init: fork failed: %s\n", strerror(errno));
        return -1;
    } else if (pid != 0) {
        msgout (INFO, "Daemon started.  Run 3ddesk to activate.\n");
        exit(0);  // bye parent
    }

    // child continues 

    setsid();
    
    chdir ("/");

    umask (0);

    return(0);
} // END daemon_init 



extern MessageManager msgmgr;


void end_program (int rv)
{
    flag_that_i_am_NOT_running();

    if (cfg)
        delete cfg;

    if (msgmgr.close() < 0)
        msgout(ERROR, "Could not delete message queue: %s\n", strerror(errno));

    exit (rv);
}



