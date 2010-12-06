//  ----------------------------------------------------------
//  3d Desktop
//
//  Allows you to switch virtual desktops (viewports) in several
//  3-dimensional ways.
// 
//  I started with a program called "OpenGL cube demo" by Chris
//  Halsall.  It has changed quite dramatically since then but it was
//  a *huge* help to me.  This is my first OpenGL program.  It was
//  coded up starting around Feb/March 2002.  Please provide feedback,
//  fixes, improvements etc. to bard@systemtoolbox.com
//
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <signal.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "3ddesk.h"

#include "move.hpp"
#include "face.hpp"
#include "arrange.hpp"
#include "faceset.hpp"
#include "xutil.hpp"
#include "event.hpp"

#include "util.h"

#include "win.hpp"

#include "message.hpp"

#include "camera.hpp"

// STL
using namespace std;
#include <iostream>
#include <list>

#include <GL/glx.h>
#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
//#include <GL/glut.h> // GLUT support library.

#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>


Config *cfg;

Camera camera;
Vector zero;

const float ones[] = { 1.0, 1.0, 1.0, 1.0 };

typedef unsigned char uchar;

// Some global variables.... 
int do_screenshot = 1;
int do_vpswitch = 1;



// Our display mode settings.
int Blend_On = 0;
int Texture_On = 1;
int Alpha_Add = 1;

int Curr_TexMode = 1;
char *TexModesStr[] = {"GL_DECAL","GL_MODULATE","GL_BLEND","GL_REPLACE"};
GLint TexModes[] = {GL_DECAL,GL_MODULATE,GL_BLEND,GL_REPLACE};



// delay between updates of the movements (in milliseconds)
long last_time;  // for movement timing


// time of last renderscene
long last_renderscene_time = 0;
// time of last keypress 
float last_keypress = 0;
// time of next "ghost keypress" when having fun
float next_action = 0;

int goto_on_flag = 0;

int initiate_destroy = 0;   // starts the z offset zoom in

Bool disable_autoacquire = false;

Bool disable_random_fun = false;

desktop_coords_t  last_coords;

time_t time_of_last_acquire = 0;

VDesktops vdesks;

MessageManager msgmgr;

Movement digit_vis;
float digit_visibility = 0.0;

Movement face_vis;
float face_visibility = 0.0;

// LIGHTS
//#define DEBUG_LIGHTING

float light_position[]= { 0.0f, 0.0f, 17.0f, 1.0f };

// diffuse light to light the front face
float light_diffuse[]=  { 1.0f, 1.0f, 1.0f, 1.0f }; 

// ambient light for a subtle background breathing effect
float light_ambient[]=  { 1.0f, 1.0f, 1.0f, 1.0f };

// changes the ambient light to look like breathing
Movement breathing;
float ambient_value = 2.0;


// this contains all the "Face" types and each holds location
// information and texture_id.  This in turn is manipulated by each
// faceset (the coords of each face not the texture_id).  This things
// lifetime is for the duration of the daemon.
FaceSet *face_set = NULL;

// this determines the coords of each face in a faceset.  it "has-a"
// faceset (passed in at creation time).  This things lifetime is for
// the duration of an arrangement or "mode".  Can be deleted and
// recreated as another arrangement many times.
Arrangement *faces = NULL;

// The class that creates the window etc.
GLWindow GLWin;

// The class that handles events.  Manages "Event"'s.  
EventManager em;

Bool end_X_event_loop;


#define STATE_ENTRY      0x00000001
#define STATE_EXIT       0x00000002
#define STATE_NORMAL     0x00000004
#define STATE_GOTO       0x00000008
#define STATE_SUSPENDED  0x00000010

#define SET_STATE(s)     (state |=  (s))
#define ISSET_STATE(s)   (state &   (s))
#define UNSET_STATE(s)   (state &= ~(s))
#define CLEAR_STATE()    (state =    0)

unsigned int state = STATE_ENTRY;


// prototypes 

static void goto_face_complete (Event *e);
static void switch_desktop_on_exit (Event *e);
static void entry_move_start_hook (Event *e);
static void exit_move_start_hook (Event *e);
static void setup_goto_face(desktop_coords_t  goto_coords, 
                            desktop_coords_t  current_coord, 
                            desktop_coords_t  current_count);
static void calc_fps(void);
static void render_scene(void);
static void mouse_pressed(unsigned int button);
static void key_pressed(KeySym key);
static void initialize_arrangements (void);
static void set_arrangement (float z_offset_distance);
static void begin_exit (void);
static desktop_coords_t wait_for_activation(void);
static void idle_func(void);
static int main_loop(void);
static void gl_init (void);
static void initialize_3ddesktop (void);
static void load_digits(void);
static void load_background_image(void);
static int background_image_is_new(char *bg_image_filename);


// Some Event hooks

// runs when a goto face reaches its destination
static void 
goto_face_complete (Event *e)
{
    goto_on_flag = 0;

    //msgout (DEBUG, "-=-=-=-=- goto_face_complete\n");

    if (!cfg->options->disable_exit_after_goto) {
        begin_exit();
    } else {
        cfg->options->disable_exit_after_goto = false;  // dont want this option "sticky"
    }

}

// runs when a goto face reaches its destination (when setup by
// setup_goto_face) and does not start an exit.
static void 
goto_face_complete2 (Event *e)
{
    goto_on_flag = 0;
}


// start the exit movement as soon as the entry movement is over when
// where doing a "goto".
static void 
goto_face_start_exit (Event *e)
{
    //msgout (DEBUG, "-=-=-=-=- goto_face_start_exit\n");
    if (!cfg->options->disable_exit_after_goto) {
        begin_exit();
    } else {
        cfg->options->disable_exit_after_goto = false;  // dont want this option "sticky"
    }

}


// runs when exit movement stops if do_vpswitch_early is false
static void 
switch_desktop_on_exit (Event *e)
{
    msgout (DEBUG, "Doing exit VD switch  c=%d   r=%d\n",
            faces->get_current_column(),
            faces->get_current_row() );
    vdesks.set_vdesktop (faces->get_current_column(),
                         faces->get_current_row() );
}

// runs when entry movement starts
static void 
entry_move_start_hook (Event *e)
{
    msgout (DEBUG, "################ ENTRY START!\n");

    digit_vis.init(&digit_visibility, 0.0, 1.0, 45, Movement::MOVE_TYPE_SMOOTH);

    face_vis.init(&face_visibility, 1.0, 0.7, 45, Movement::MOVE_TYPE_SMOOTH);

    faces->entry_movement_start();
}

// runs when exit movement starts
static void 
exit_move_start_hook (Event *e)
{
    msgout (DEBUG, "################ EXIT START!\n");

    digit_vis.init(&digit_visibility, 1.0, 0.0, 45, Movement::MOVE_TYPE_SMOOTH);

    face_vis.init(&face_visibility, 0.7, 1.0, 45, Movement::MOVE_TYPE_SMOOTH);

    faces->exit_movement_start();
}



static void 
setup_goto_face(desktop_coords_t  goto_coords, 
                desktop_coords_t  current_coord, 
                desktop_coords_t  current_count)
{
    msgout (DEBUG, "**** setting up goto = %d x %d\n", goto_coords.column, goto_coords.row);

    switch (goto_coords.column) 
    {
    case GOTO_FACE_RIGHT:
        goto_coords.column = current_coord.column + 1;
        if (goto_coords.column >= current_count.column) 
            goto_coords.column = 0;
        break;

    case GOTO_FACE_LEFT:
        goto_coords.column = current_coord.column - 1;
        if (goto_coords.column < 0) 
            goto_coords.column = current_count.column - 1;
        break;

    case NO_GOTO:
        goto_coords.column = current_coord.column;
        break;

    default:
        goto_coords.column--;  // change 1 thru max to 0 thru max-1
    }

    switch (goto_coords.row) 
    {
    case GOTO_FACE_DOWN:
        goto_coords.row = current_coord.row + 1;
        if (goto_coords.row >= current_count.row) 
            goto_coords.row = 0;
        break;

    case GOTO_FACE_UP:
        goto_coords.row = current_coord.row - 1;
        if (goto_coords.row < 0) 
            goto_coords.row = current_count.row - 1;
        break;

    case NO_GOTO:
        goto_coords.row = current_coord.row;
        break;

    default:
        goto_coords.row--;
    }

    goto_on_flag = 1;

    msgout (DEBUG, "**** complete - going with = %d x %d\n", goto_coords.column, goto_coords.row);

    desktop_coords_t dc(goto_coords);

    SET_STATE(STATE_GOTO);
    faces->goto_face (dc);

    em.add_event (GOTO_FACE_COMPLETE, goto_face_complete2);

    em.add_event (ENTRY_MOVEMENT_STOP, goto_face_start_exit);

} // END setup_goto_face


typedef struct portion_control_s {
    int inited;
    int x_segment_size;
    int y_segment_size;
    int n_x_seg;
    int n_y_seg;
    int current_x_seg;
    int current_y_seg;
} portion_control_t;

//#define SCAN_MODE

#ifdef SCAN_MODE

static void
init_portion_control (portion_control_t *pc)
{
    pc->x_segment_size = 128;
    pc->y_segment_size = 128;
    pc->n_x_seg = (int)floor(GLWin.get_screen_width() / pc->x_segment_size);
    pc->n_y_seg = (int)floor(GLWin.get_screen_height() / pc->y_segment_size);

    //msgout (DEBUG, "*************** %d x %d\n", pc->n_x_seg, pc->n_y_seg);

    pc->current_x_seg = 0;
    pc->current_y_seg = 0;

    pc->inited = 1;
}

static void
grab_portion_of_screen (portion_control_t *pc)
{
    if (!pc->inited)
        init_portion_control(pc);

    int x1 = pc->current_x_seg * pc->x_segment_size;
    int x2 = pc->current_x_seg * pc->x_segment_size + pc->x_segment_size;
    int y1 = pc->current_y_seg * pc->y_segment_size;
    int y2 = pc->current_y_seg * pc->y_segment_size + pc->y_segment_size;

    msgout (DEBUG, "GRABBING segment %d x %d\n", 
            pc->current_x_seg, pc->current_y_seg);

    GLWin.grab_screenshot_data_portion (x1, y1, x2, y2);

    pc->current_x_seg++;
    if (pc->current_x_seg == pc->n_x_seg) {

        msgout (DEBUG, "reached max X %d\n", pc->n_x_seg);

        pc->current_x_seg = 0;
        pc->current_y_seg++;
        if (pc->current_y_seg == pc->n_y_seg) 
            pc->current_y_seg = 0;
    }

    
    //= get_randomi(0, pc->n_x_seg - 1);
    //= get_randomi(0, pc->n_y_seg - 1);

} // END grab_portion_of_screen 

#endif // SCAN_MODE 





// ------
#define FRAME_RATE_SAMPLES 100   // at 100 fps this will be about every second we'll get a value
int   framecount = 0;
float framerate = 0;

static void 
calc_fps(void)
{
    static long last = 0;
    long now;
    double delta; // the number of seconds that we took to draw FRAME_RATE_SAMPLES frames

    if (++framecount >= FRAME_RATE_SAMPLES) {
        now = get_milli_time();
        delta = (double)(now - last) / (double)1000.0;
        //now  = clock();
        //delta = (now - last) / (float) CLOCKS_PER_SEC;
        last = now;

        framerate = (float) FRAME_RATE_SAMPLES / delta;
        framecount = 0;
        msgout(DEBUG, "FPS: %lf\n", framerate);
    }
} // END calc_fps


#if 0
// ------
// Frames per second (FPS) statistic variables and routine.
#define FRAME_RATE_SAMPLES 50
int   FrameCount = 0;
float FrameRate = 0;

static void 
calc_fps(void)
{
    static clock_t last=0;
    clock_t now;
    float delta;

    if (++FrameCount >= FRAME_RATE_SAMPLES) {
        now  = clock();
        delta= (now - last) / (float) CLOCKS_PER_SEC;
        last = now;

        FrameRate = FRAME_RATE_SAMPLES / delta;
        FrameCount = 0;
        msgout(DEBUG, "FPS: %lf\n", FrameRate);
    }
} // END calc_fps
#endif

uint digit_texture;


static void draw_digits (int r, int c);
static void draw_desktop_number(int num);
static void draw_digit (int d, float x_trans, float y_trans);
static void draw_x (float x_trans, float y_trans);
static void set_color (int color, float level);


static void
set_color (int color, float level) 
{
    switch (color) {
    case COLOR_RED:
        glColor4f(level, 0, 0, 0); 
        break;
    case COLOR_GREEN:
        glColor4f(0, level, 0, 0);
        break;
    case COLOR_BLUE:
        glColor4f(0, 0, level, 0);
        break;
    case COLOR_LIGHTBLUE:
        glColor4f(0, level, level, 0);
        break;
    case COLOR_WHITE:
        glColor4f(level, level, level, 0);
        break;
    case COLOR_GRAY:
        glColor4f(level/2, level/2, level/2, 0);
        break;
    case COLOR_PURPLE:
        glColor4f(level, 0, level, 0);
        break;
    case COLOR_YELLOW:
        glColor4f(level, level, 0, 0);
        break;
    default: // white
        glColor4f(level, level, level, 0);
        break;
    }
}


static void 
draw_desktop_number(int num)
{
    float digit_size = cfg->options->digit_size;
  
    // where to draw to 
    float centre_x = GLWin.get_width() / 2 ;
    float centre_y = GLWin.get_height() - (digit_size / 2) ;
  
    // decode to digits
 
    int numdigits = (int) log10((float)num);
 
    float current_x = centre_x - (digit_size * numdigits / 2);
  
    while(num >= 10) {
	draw_digit(num / 10, 
		   current_x, 
		   centre_y);
	current_x += digit_size;
  	num = num % 10; 
    }

    draw_digit(num, 
               current_x, 
               centre_y);
}



static void
draw_digits (int r, int c)
{
    float digit_size = cfg->options->digit_size;

    draw_x (GLWin.get_width() / 2,
            GLWin.get_height() - (digit_size / 2) );

    draw_digit (r, 
                GLWin.get_width() / 2 - (digit_size),
                GLWin.get_height() - (digit_size / 2) );

    draw_digit (c, 
                GLWin.get_width() / 2 + (digit_size),
                GLWin.get_height() - (digit_size / 2) );

}


static void
draw_x (float x_trans, float y_trans)
{
    float x1 = .5, y1 = .5, x2 = .75, y2 = .75;

    float w = cfg->options->digit_size/2.0;

    // assume we are already in projection matrix
    glTranslatef(x_trans, y_trans, 0.0f);

    set_color (cfg->options->digit_color, digit_visibility);

    glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, digit_texture );

    glBegin(GL_QUADS);
    
    glTexCoord2f (x1, y2);
    glVertex3f(-w,-w,0);
    
    glTexCoord2f (x2, y2);
    glVertex3f(w,-w,0);
    
    glTexCoord2f (x2, y1);
    glVertex3f(w,w,0);
    
    glTexCoord2f (x1, y1);
    glVertex3f(-w,w,0);
    
    glEnd();
}


// this is where the digit is located in the texture
float digit_texture_coords[] = 
{ .25	, .5, 
  0	, 0, 
 .25	, 0, 
 .5	, 0, 
 .75	, 0, 
 0	, .25, 
 .25	, .25, 
 .5	, .25, 
 .75	, .25, 
 0	, .5, 
 .25	, .5 };

static void
draw_digit (int d, float x_trans, float y_trans)
{
    float x1, y1, x2, y2;

    if (d >= 10)
        return ;

    x1 = digit_texture_coords[d * 2];
    y1 = digit_texture_coords[d * 2 + 1];
   
    x2 = x1 + 0.25;
    y2 = y1 + 0.25;

    //msgout (DEBUG, "oooooooo d=%d  x1=%f  y1=%f  x2=%f  y2=%f\n",
    //        d, x1, y1, x2, y2);


    float w = cfg->options->digit_size/2.0;

    // assume we are in projection matrix already
    glTranslatef(x_trans, y_trans, 0.0f);

    set_color (cfg->options->digit_color, digit_visibility);

    glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, digit_texture );

    glBegin(GL_QUADS);
    
    glTexCoord2f (x1, y2);
    glVertex3f(-w,-w,0);
    
    glTexCoord2f (x2, y2);
    glVertex3f(w,-w,0);
    
    glTexCoord2f (x2, y1);
    glVertex3f(w,w,0);
    
    glTexCoord2f (x1, y1);
    glVertex3f(-w,w,0);
    
    glEnd();

}

static bool
allow_blending(void)
{
    return (faces->allow_blending() && Blend_On);
}


static void 
setup_gl_modes(void)
{
    if (Texture_On)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);

    // If we're blending, we don't want z-buffering.
    if (allow_blending()) {
        glDisable(GL_DEPTH_TEST); 
        glEnable(GL_BLEND);
    } else { 
        glEnable(GL_DEPTH_TEST); 
        glDisable(GL_BLEND); 
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,TexModes[Curr_TexMode]);

    // only effects gl_blend
    //float text_env_color[] = { .1, .1, .1, .1 };
    //glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR, text_env_color);

}  // setup_gl_modes


uint background_texture;

static void
draw_background_image(void)
{
    float screen_width = GLWin.get_width();
    float screen_height = GLWin.get_height();
        
    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    glLoadIdentity();

    glOrtho(0,screen_width,0,screen_height,-1.0,1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glDisable(GL_DEPTH_TEST); 
    glDepthMask(GL_FALSE);

    //glTranslatef(0.0f, 0.0f, 0.0f);
        
    // leave it up to the creator of the image to keep it from being too bright
    //glColor4f(.5,.5,.5,.5);
    glColor4f(1.0,1.0,1.0,1.0);

    glBindTexture(GL_TEXTURE_2D, background_texture);

    glBegin(GL_QUADS);
     glTexCoord2f (0.0f, 1.0f);
     glVertex3f(        0.0f,          0.0f, 0.0f);

     glTexCoord2f (0.0f, 0.0f);
     glVertex3f(        0.0f, screen_height, 0.0f);

     glTexCoord2f (1.0f, 0.0f);
     glVertex3f(screen_width, screen_height, 0.0f);

     glTexCoord2f (1.0f, 1.0f);
     glVertex3f(screen_width,          0.0f, 0.0f);
    glEnd();

    glMatrixMode(GL_PROJECTION);    

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

} // draw_background_image



//
// Some hacky performance measurement stuff

//#define PERF_DEBUG 1
#if defined PERF_DEBUG

struct timeit_data {
    long usecs;
    long count;
    long total;
};

#define NUM_TIMEIT_SLOTS 20
struct timeit_data timeit[NUM_TIMEIT_SLOTS];

#define TIMEIT_INIT()       do { memset(timeit, 0, sizeof(struct timeit_data) * NUM_TIMEIT_SLOTS); } while (0);
#define TIMEIT_START(i)      do {  \
                               struct timeval tv; \
                               gettimeofday(&tv, 0); \
                               timeit[(i)].usecs = tv.tv_sec * 1000000 + tv.tv_usec; \
                             } while (0);
#define TIMEIT_STOP(i)       do { \
                               struct timeval tv; \
                               gettimeofday(&tv, 0); \
                               long delta = (tv.tv_sec * 1000000 + tv.tv_usec)  -  timeit[(i)].usecs; \
                               if ((timeit[(i)].count < 10) || (delta < 2 *(timeit[(i)].total / timeit[(i)].count))) { \
                                   timeit[(i)].total += delta; /* this smooths out the running average a bit */  \
                               } \
                               timeit[(i)].count++;           \
                             } while (0);

#define TIMEIT_PRINT(i, s)   do {                                      \
                               printf("Running average for %s(%d): %lf useconds (%ld samples)\n", \
                                      (s), (i), (double)(timeit[(i)].total) / (double)(timeit[(i)].count), timeit[(i)].count); \
                             } while(0);

#else 

#define TIMEIT_INIT()
#define TIMEIT_START(i)
#define TIMEIT_STOP(i)
#define TIMEIT_PRINT(i,s)

#endif // if defined PERF_DEBUG

// ------
// Routine which actually does the drawing
static void
render_scene(void)
{
    //char buf[80]; // For our strings.
    long current_time;

    current_time = get_milli_time();

    // this gives us semi-precise/consistent timing of the movements
    // 
    if (current_time - last_time > cfg->options->animation_speed) {

        if (ISSET_STATE(STATE_EXIT)) {

            //if (!cfg->options->entry_exit_movement || faces->is_exit_movement_done()) {

            if (faces->is_exit_movement_done()) {
                // quit (the z offset was changed to match the screen)

                if (!ISSET_STATE(STATE_GOTO)) {  // dont exit if we're still goto'ing
                    UNSET_STATE(STATE_EXIT);
                    end_X_event_loop = True;
                    em.trigger_event (EXIT_MOVEMENT_STOP);
                    return;
                }
                
            } else {
                faces->entry_exit_move();
            }

        } else if (ISSET_STATE(STATE_ENTRY)) {

            if ( faces->is_entry_movement_done() ) {
                UNSET_STATE(STATE_ENTRY);
                SET_STATE(STATE_NORMAL);
                em.trigger_event (ENTRY_MOVEMENT_STOP);

            } else {
                faces->entry_exit_move();

            }

        } 

        // change state to normal from goto if done goto
        if (ISSET_STATE(STATE_GOTO)) {
            if ( !faces->in_goto() ) {
                UNSET_STATE(STATE_GOTO);
                SET_STATE(STATE_NORMAL);
                em.trigger_event (GOTO_FACE_COMPLETE);
            }
        }

        
        if (cfg->options->use_breathing) {
            if (breathing.at_destination()) {
                breathing.init(&ambient_value, 1.7, 3.0, 200, Movement::MOVE_TYPE_SINE);
            } else {
                breathing.change_a_bit();
                light_ambient[0] = light_ambient[1] = light_ambient[2] = ambient_value;
            }

        }

        if (!digit_vis.at_destination())
            digit_vis.change_a_bit();

        if (!face_vis.at_destination())
            face_vis.change_a_bit();

        faces->set_transparency(face_visibility);

        faces->do_movement();

        last_time = get_milli_time();

    } // end animation step
 
    // FIXME: use event to accomplish this?

    // after some seconds start some fun ;)
    if (!disable_random_fun && 
        cfg->options->random_fun_delay &&
        current_time - last_keypress > cfg->options->random_fun_delay * 1000) 
    {
        if ((unsigned long)current_time >= (unsigned long)next_action) {

            //msgout(DEBUG, "having some fun...\n");
            faces->goto_random_face();
            
            next_action = current_time + get_randomi(350, 600);
        }
    }

    setup_gl_modes();

    glClear(GL_COLOR_BUFFER_BIT | (allow_blending() ? 0 : GL_DEPTH_BUFFER_BIT));

    if (cfg->use_background_image) {

        draw_background_image();

        setup_gl_modes();  // have to do this again as draw_bg_image changed it
    }

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();

    // zero position of camera (FIX THIS!)
    camera.position = zero;
    camera.viewvector = zero;

    // adjust the faces the necessary amount
    faces->position_faces();

    faces->render();

    glPopMatrix();

    //
    //  Now do some lighting
    // 
    glPushMatrix();

    glLoadIdentity();

    faces->position_lighting();


// for debugging
#if defined DEBUG_LIGHTING
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(15.0);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(15.0);
    glBegin(GL_POINTS);
    glVertex4fv(light_position);
    glEnd();
#endif

    // Lighting 

    // LIGHT1 is a diffuse light on the front face
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  ones);
    glLightfv(GL_LIGHT1, GL_SPECULAR,  ones);
    glEnable (GL_LIGHT1);

    // LIGHT2 is ambient background light
    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient);
    glEnable (GL_LIGHT2);


    glPopMatrix();

    if (cfg->options->show_digits) {

        // We need to change the projection matrix for the text rendering.  
        glMatrixMode(GL_PROJECTION);

        glPushMatrix();

        glLoadIdentity();

        glOrtho(0,GLWin.get_width(),0,GLWin.get_height(),-1.0,1.0);

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        // Lit or textured text looks awful.
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        // We don't want depth-testing either.
        glDisable(GL_DEPTH_TEST); 



        // Now we want to render the calulated FPS at the top.
   
        // To ease, simply translate up.  Note we're working in screen
        // pixels in this projection.
   
        if (faces->get_number_of_columns() > 1 &&
            faces->get_number_of_rows() > 1) 
        {
            draw_digits (faces->get_current_row() + 1,
                         faces->get_current_column() + 1);
        } else {
            int n = 1;
            if (faces->get_number_of_columns() > 1)
                n = faces->get_current_column() + 1;
            else
                n = faces->get_current_row() + 1;

            draw_desktop_number (n);
        }

#if 0  // use a string rather then the digit

        // FIXME: do this if the digit texture didn't load

        char buf[50];

        // use N x N if both rows and columns otherwise use juse N
        // (representing columns or rows depending on which is being
        // used)
        if (faces->get_number_of_columns() > 1 &&
            faces->get_number_of_rows() > 1) 
        {
            sprintf(buf,"%d x %d", 
                    faces->get_current_row() + 1,
                    faces->get_current_column() + 1);
        } else {
            int n = 1;
            if (faces->get_number_of_columns() > 1)
                n = faces->get_current_column() + 1;
            else
                n = faces->get_current_row() + 1;

            sprintf(buf,"%d", n);
        }
        
        glRasterPos2i(6,0);
        ourPrintString(GLUT_BITMAP_TIMES_ROMAN_24,buf);
#endif

        glMatrixMode(GL_PROJECTION);

        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

    }

    glXSwapBuffers(GLWin.get_display(), GLWin.get_window());

    // And collect our statistics.
    if (cfg->options->show_fps)
        calc_fps();

    last_renderscene_time = get_milli_time();

} // END render_scene



static void
mouse_pressed(unsigned int button)
{
    last_keypress = get_milli_time();

    //msgout (DEBUG, "button pressed %d - 0x%x\n", button, button);

    list<Config::mousebinding *>::iterator k;
    for (k = cfg->mousebindings.begin(); k != cfg->mousebindings.end(); k++) {
        if ((*k)->button == button) {
            switch ((*k)->action) {
            case Config::LEFT:
                faces->goto_left();
                break;
            case Config::RIGHT:
                faces->goto_right();
                break;
            case Config::DOWN:
                faces->goto_down();
                break;
            case Config::UP:
                faces->goto_up();
                break;
            case Config::SELECT:
                begin_exit();
                break;
            }
            return;
        }
    }


    switch (button) {
    case 1: // left
    	if (cfg->options->alt_mousebuttons)
            begin_exit();
        else
            if (cfg->options->swap_mousebuttons)
                faces->goto_right();
            else
                faces->goto_left();
        break;

    case 3: // right
        if (cfg->options->alt_mousebuttons) {
            SET_STATE(STATE_GOTO);
            faces->goto_face(last_coords);
            begin_exit();
        } else 
            if (cfg->options->swap_mousebuttons)
                faces->goto_left();
            else
                faces->goto_right();
        break;

    case 2: // middle
        if (cfg->options->alt_mousebuttons) {
            SET_STATE(STATE_GOTO);
            faces->goto_face(last_coords);
        } else {
            begin_exit();
        }
        break;


    case 4: // mouse wheel up
        if (cfg->options->reverse_mousewheel)
            faces->goto_left();
        else
            faces->goto_right();
        break;

    case 5: // mouse wheel down
        if (cfg->options->reverse_mousewheel)
            faces->goto_right();
        else
            faces->goto_left();
        break;


    case 6: // extra button on some mice, do we use this?
        if (cfg->options->swap_mousebuttons)
            faces->goto_left();
        else
            faces->goto_right();
        break;

    case 7: // extra button on some mice, do we use this?
        if (cfg->options->swap_mousebuttons)
            faces->goto_right();
        else
            faces->goto_left();
        break;

    }

} // END mouse_pressed


// ------
// Callback function called when a normal key is pressed.

static void
key_pressed(KeySym key)
{
    static KeySym last_key_pressed = 0;

    if (cfg->disable_keys_in_goto &&
        goto_on_flag && 
        !cfg->options->disable_exit_after_goto)
        return;

    last_keypress = get_milli_time();

    next_action = 0;

    list<Config::keybinding *>::iterator k;
    for (k = cfg->keybindings.begin(); k != cfg->keybindings.end(); k++) {
        if ((*k)->key == key) {
            switch ((*k)->action) {
            case Config::LEFT:
                faces->goto_left();
                break;
            case Config::RIGHT:
                faces->goto_right();
                break;
            case Config::DOWN:
                faces->goto_down();
                break;
            case Config::UP:
                faces->goto_up();
                break;
            case Config::SELECT:
                begin_exit();
                break;
            }
            return;
        }
    }

    switch (key) {
    case 130: case 98: // B - Blending.
        Blend_On = Blend_On ? 0 : 1; 
        if (!allow_blending())
            glDisable(GL_BLEND); 
        else
            glEnable(GL_BLEND);
        break;

    case 108: case 76:  // L - last desktop
        SET_STATE(STATE_GOTO);
        faces->goto_face(last_coords);
        break;

    case 109: case 77:  // M - Mode of Blending
        if ( ++ Curr_TexMode > 3 )
            Curr_TexMode=0;
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,TexModes[Curr_TexMode]);
        msgout (DEBUG, "glTexEnv mode=%s (%d)\n", TexModesStr[Curr_TexMode],
                TexModes[Curr_TexMode]);
        break;

    case 116: case 84: // T - Texturing.
        Texture_On = Texture_On ? 0 : 1; 
        break;

    case 97: case 65:  // A - Alpha-blending hack.
        Alpha_Add = Alpha_Add ? 0 : 1; 
        break;

    case 114: case 82: // R - random
        faces->goto_random_face();
        break;

    case 102: case 70: // F - Fun Mode
        disable_random_fun = disable_random_fun ? 0 : 1;
        break;

// for debugging best light position...
#if defined DEBUG_LIGHTING 
    case 119: // w
        light_position[0] -= 0.5;
        msgout (DEBUG, "light x at %f\n", light_position[0]);
        break;
    case 101: // e
        light_position[0] += 0.5;
        msgout (DEBUG, "light x at %f\n", light_position[0]);
        break;
    case 115: // s 
        light_position[1] -= 0.5;
        msgout (DEBUG, "light y at %f\n", light_position[1]);
        break;
    case 100: // d 
        light_position[1] += 0.5;
        msgout (DEBUG, "light y at %f\n", light_position[1]);
        break;
    case 120: // x
        light_position[2] -= 0.5;
        msgout (DEBUG, "light z at %f\n", light_position[2]);
        break;
    case 99: // c
        light_position[2] += 0.5;
        msgout (DEBUG, "light z at %f\n", light_position[2]);
        break;
#endif

    case 48:
        if (last_key_pressed == key) {
            // second time this key was hit so exit.
            begin_exit();
            break;
        }
        last_key_pressed = key;

        SET_STATE(STATE_GOTO);
        faces->goto_face(9);
        break;
        
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
        if (last_key_pressed == key) {
            // second time this key was hit so exit.
            begin_exit();
            break;
        }

        last_key_pressed = key;

        if (initiate_destroy) {

            initiate_destroy = 0;
            faces->exit_movement_cancel();

            // FIXME: replace with event something or other
            SET_STATE(STATE_GOTO);
            faces->goto_face(key - 49);

        } else {
            SET_STATE(STATE_GOTO);
            faces->goto_face(key - 49);
        }

        break;

#define  KEY_ENTER  0xff0d
#define  KEY_UP     0xff52
#define  KEY_DOWN   0xff54
#define  KEY_RIGHT  0xff53
#define  KEY_LEFT   0xff51
#define  KEY_ESCAPE 0xff1b

    case KEY_ENTER: 
    case KEY_ESCAPE:
    case 113:  // 'q'
    case 32:   // space
        // We're outta here.

        begin_exit();

        // the z_offset translation starts and is carried out by the
        // render function where we exit when it completes
        break;

    case KEY_LEFT:

        faces->goto_left();

        break;

    case KEY_RIGHT:

        faces->goto_right();

        break;

    case KEY_DOWN:

        faces->goto_down();

        break;

    case KEY_UP:

        faces->goto_up();

        break;

    default:
        msgout (DEBUG, "KP: No action for %ld / 0x%lx.\n", key, key);
        break;
    }

} // END key_pressed



Arrangement *arrangement_array[FACE_TYPE_LAST];

// assumes 'a' is Arrangement *a;
#define  ALLOC_ARRANGEMENT(enum_name, type)                     \
    if (arrangement_array[enum_name])                           \
        delete (arrangement_array[enum_name]);                  \
    a = new type (vdesks,                                       \
                  face_width, face_height,                      \
                  face_set, cfg);                               \
    if (!a) {                                                   \
        msgout(ERROR, "out of memory\n");                       \
        end_program (1);                                        \
    }                                                           \
    arrangement_array[enum_name] = a;


static void
initialize_arrangements (void) 
{

    memset(arrangement_array, 0, sizeof(arrangement_array));
    

#if 0
    float aspect;
    float face_height, face_width;

    Arrangement *a = NULL;

    aspect = (float)GLWin.get_screen_width() / (float)GLWin.get_screen_height();
    face_width = 4.0;
    face_height = face_width/aspect;

    static bool first_time = true;
    if (first_time) {
        memset(arrangement_array, 0, sizeof(arrangement_array));
        first_time = false;
    }
    
    // tediously allocate each arrangement type and stuff into
    // "arrangement_array"

    ALLOC_ARRANGEMENT(FACE_TYPE_CAROUSEL, CarouselArrangement);
    ALLOC_ARRANGEMENT(FACE_TYPE_CYLINDER, CylinderArrangement);
    ALLOC_ARRANGEMENT(FACE_TYPE_VIEWMASTER, ViewMasterArrangement);
    ALLOC_ARRANGEMENT(FACE_TYPE_PRICEISRIGHT, PriceIsRightArrangement);
    ALLOC_ARRANGEMENT(FACE_TYPE_LINEAR, LinearArrangement);
    ALLOC_ARRANGEMENT(FACE_TYPE_FLIP, FlipArrangement);
#endif

} // END initialize_arrangements


static Arrangement *
demand_load_arrangement (FaceType_t type)
{
    
    Arrangement *f = arrangement_array [type];
    if (!f) {
        msgout (DEBUG, "DDDDDDDDDDDDDD demand loading %d\n", type);

        float aspect;
        float face_height, face_width;
        
        aspect = (float)GLWin.get_screen_width() / (float)GLWin.get_screen_height();
        face_width = 4.0;
        face_height = face_width/aspect;
        
        switch (type) {
        case FACE_TYPE_CAROUSEL:
            f = new CarouselArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
#if !defined (ONE_ARRANGEMENT_ONLY)
        case FACE_TYPE_CYLINDER:
            f = new CylinderArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
        case FACE_TYPE_VIEWMASTER:
            f = new ViewMasterArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
        case FACE_TYPE_PRICEISRIGHT:
            f = new PriceIsRightArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
        case FACE_TYPE_LINEAR:
            f = new LinearArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
        case FACE_TYPE_FLIP:
            f = new FlipArrangement (vdesks, face_width, face_height, face_set, cfg, &camera);
            break;
#endif
        default:
            f = NULL;
        }

        if (!f) {
            msgout(ERROR, "out of memory\n");
            end_program(-1);
        }
        arrangement_array[type] = f;
    }
    return f;
}


static void
set_arrangement (float z_offset_distance) 
{
    FaceType_t type = cfg->options->face_type;

    if (type == FACE_TYPE_RANDOM)
        type = (FaceType_t)get_randomi (1, FACE_TYPE_LAST - 1);


    if (type == FACE_TYPE_NONE)
        if (z_offset_distance)
            faces->set_z_offset_distance(z_offset_distance);


    if (type < 0 || type >= FACE_TYPE_LAST) {
        msgout (ERROR, "BUG: Unknown face type (?)\n");
        end_program(1);
    }
        

    // type is valid - demand load the arrangement
    if ((faces = demand_load_arrangement(type)) == NULL) {
        msgout (ERROR, "BUG: could not demand load arrangement %d!\n", type);
        end_program(-1);
    }
    
    faces->calculate_faces(vdesks);
    
    //msgout (DEBUG, "xxxxxxxx Setting %d (zod %f) at 0x%x\n", 
    //        type, z_offset_distance, faces);
    
    
    if (z_offset_distance)
        faces->set_z_offset_distance(z_offset_distance);

} // END set_arrangement



static void 
begin_exit (void)
{
    if (    faces->cant_exit_in_goto()
         && faces->in_goto())
    {
        // if no "zooming" then we don't want to exit until the goto
        // face is complete
        msgout (DEBUG, "cant exit in goto....\n");
        SET_STATE(STATE_GOTO);
        em.remove_event (GOTO_FACE_COMPLETE, goto_face_complete2);
        em.add_event (GOTO_FACE_COMPLETE, goto_face_complete);
        return;
    }

    SET_STATE(STATE_EXIT);
    em.trigger_event (EXIT_MOVEMENT_START);
    initiate_destroy = 1;

    if (do_vpswitch) {

        if (cfg->early_desktop_switch) {
            msgout (DEBUG, "Doing VD switch  c=%d   r=%d\n",
                    faces->get_current_column(),
                    faces->get_current_row() );
            vdesks.set_vdesktop (faces->get_current_column(),
                                 faces->get_current_row() );
        } else {
            em.add_event (EXIT_MOVEMENT_STOP, switch_desktop_on_exit);
        }
    }

    // the z_offset translation starts and is carried out by the
    // render function where we exit when it completes
}






portion_control_t portion_control;


static desktop_coords_t
wait_for_activation(void)
{
    //threedeedesk_command_t  msg;

    int done = 0;
    int col_count = 1;
    int row_count = 1;
    int current_col = 0;
    int current_row = 0;

    desktop_coords_t goto_coords;

    goto_coords.column = NO_GOTO;
    goto_coords.column = NO_GOTO;

    CLEAR_STATE();
    SET_STATE(STATE_SUSPENDED);

    disable_autoacquire = false;
    if (cfg->autoacquire)
        alarm(cfg->autoacquire);

    do {

#ifdef SCAN_MODE
        do {

            // must setup current_etc for screen portion grab. this
            // should work by registering notifiers so we can change
            // the values only when we need to rather then "polling"
            // for changes
            vdesks.get_vdesktop_info (&current_col, &current_row, &col_count, &row_count);

            // do some work here....
            grab_portion_of_screen (&portion_control);

            face_set->load_texture_data(current_col + (current_row * col_count), 
                                        cfg->texture_size,
                                        GLWin.get_screenshot_data_ptr() );

            //msgout (DEBUG, ".....grabbed\n");
            sleep_ms (20);


            // non-blocking - returns -1 error and 0 for success or NOMSG
            if (msgmgr.recieve(MessageManager::NOWAIT) < 0) {
                msgout (ERROR, "msgrcv failed: %s\n", strerror(errno));
                continue;
            }

        } while (errno == ENOMSG);
#else
        if (msgmgr.flush() < 0) {
            msgout (ERROR, "flush failed: %s\n", strerror(errno));
            end_program(-1);
        }

        // should block until message received
        if (msgmgr.recieve() < 0) {
            msgout (ERROR, "msgrcv failed: %s\n", strerror(errno));
            end_program(-1);
        }

#endif


        msgout (DEBUG, "MSG: %ld\n", msgmgr.msg.mtype);

        disable_autoacquire = true;

        // must get this now in case the current desktop was changed by
        // other means
        vdesks.get_vdesktop_info (&current_col, &current_row, &col_count, &row_count);

        msgout(DEBUG, "####### current %d x %d\n", current_col, current_row);
        faces->set_current_face (current_col, current_row);

        switch (msgmgr.msg.mtype) {
        case MTYPE_STOP:
            msgout (DEBUG, "-- quiting\n");
            TIMEIT_PRINT(1, "render_scene    ");
            end_program(0);

            break;
        case MTYPE_ACQUIRE_CURRENT:

            if (msgmgr.msg.reload)
                cfg->reload();

            if (GLWin.grab_screenshot_data() < 0) {
                msgout (ERROR, "couldn't grab screen image\n");
                end_program(-1);
            }
            
            face_set->load_texture_data(current_col + (current_row * col_count), 
                                        cfg->texture_size,
                                        GLWin.get_screenshot_data_ptr() );
            
            continue;  // don't activate
            
            break;
        case MTYPE_ACQUIRE:

            if (msgmgr.msg.reload)
                cfg->reload();

            // loop through all desktops and grab them.
            int i, j;
            for (i = 0; i < row_count; i++) {
                for (j = 0; j < col_count; j++) {
                
                    vdesks.set_vdesktop (j, i);
                    
                    if (msgmgr.msg.acquire_sleep > 1) {
                        msgout (DEBUG, "acquire sleep is %d\n", msgmgr.msg.acquire_sleep);
                        sleep_ms (msgmgr.msg.acquire_sleep);
                    }
                    
                    if (GLWin.grab_screenshot_data() < 0) {
                        msgout (ERROR, "couldn't grab screen image\n");
                        end_program(-1);
                    }
                    
                    face_set->load_texture_data(j + (i * col_count), 
                                                cfg->texture_size,
                                                GLWin.get_screenshot_data_ptr() );
                }
            }
            // go back to current
            vdesks.set_vdesktop (current_col, current_row);
            
            continue;  // don't activate

            break;
        case MTYPE_JUSTSWITCH:
            
            goto_coords.column = msgmgr.msg.goto_column;
            goto_coords.row = msgmgr.msg.goto_row;
            
            if (goto_coords.column == NO_GOTO &&
                goto_coords.row    == NO_GOTO)
                continue;  // do nothing

            // assign as current col/row if no_goto
            if (goto_coords.column == NO_GOTO)
                goto_coords.column = current_col + 1;

            if (goto_coords.row == NO_GOTO)
                goto_coords.row = current_row + 1;

            // if we are asked to goto the current row and column then
            // do nothing
            if (goto_coords.column == (current_col + 1)
                && goto_coords.row == (current_row + 1))
                continue;

            // check bounds on goto column
            if ( goto_coords.column < 1
                 || goto_coords.column > col_count )
            {
                msgout (DEBUG, "Ignoring out of bounds goto column of %d\n", goto_coords.column);
                continue;
            }


            // check bounds on goto row
            if ( goto_coords.row < 1
                 || goto_coords.row > row_count )
            {
                msgout (DEBUG, "Ignoring out of bounds goto row of %d\n", goto_coords.row);
                continue;
            }

            // capture the current desktop
            if (GLWin.grab_screenshot_data() < 0) {
                msgout (ERROR, "couldn't grab screen image\n");
                end_program(-1);
            }
                    
            face_set->load_texture_data(current_col + (current_row * col_count), 
                                        cfg->texture_size,
                                        GLWin.get_screenshot_data_ptr() );
            
            // now go to the specified desktop
            vdesks.set_vdesktop (goto_coords.column-1, goto_coords.row-1);

            // now capture this desktop so we have an updated image
            if (GLWin.grab_screenshot_data() < 0) {
                msgout (ERROR, "couldn't grab screen image\n");
                end_program(-1);
            }
                    
            face_set->load_texture_data((goto_coords.column-1) + ((goto_coords.row-1) * col_count), 
                                        cfg->texture_size,
                                        GLWin.get_screenshot_data_ptr() );
            
            // we are done and do not activate.
            continue;

            break;
        case MTYPE_ACTIVATE:

            if (strlen(msgmgr.msg.view)) {
                // client specified a view - ignore cmd line params except goto

                cfg->reload(); // always reload cfg if view specified

                cfg->set_config_set (msgmgr.msg.view);

                goto_coords.column = cfg->options->goto_column;
                goto_coords.row = cfg->options->goto_row;

            } else {

                cfg->reload(); // reload in case "default" changed
                
                cfg->set_config_set ("default");

                // set is cmd line (default) and let other client
                // specified options override anything

                if (msgmgr.msg.zoom != -1)
                    cfg->options->entry_exit_movement = msgmgr.msg.zoom;

                if (msgmgr.msg.face_change_steps)
                    cfg->options->face_change_steps = msgmgr.msg.face_change_steps;

                if (msgmgr.msg.zoom_steps)
                    cfg->options->zoom_steps = msgmgr.msg.zoom_steps;

                msgout (DEBUG, "ZOOM is %d\n", cfg->options->entry_exit_movement);

                if (msgmgr.msg.face_type != FACE_TYPE_NONE)
                    cfg->options->face_type = msgmgr.msg.face_type;


                goto_coords.column = msgmgr.msg.goto_column;
                goto_coords.row = msgmgr.msg.goto_row;

            }

            if (msgmgr.msg.disable_exit_after_goto)
                cfg->options->disable_exit_after_goto = true;

            if (msgmgr.msg.disable_random_fun)
                disable_random_fun = 0;

            if (msgmgr.msg.reverse_mousewheel)
                cfg->options->reverse_mousewheel = true;
            if (msgmgr.msg.swap_mousebuttons)
                cfg->options->swap_mousebuttons = true;
            if (msgmgr.msg.alt_mousebuttons)
                cfg->options->alt_mousebuttons = true;

            // if we are asked to goto the current row and column then
            // do nothing
            if (goto_coords.column == (current_col + 1)
                && goto_coords.row == (current_row + 1))
                continue;

            // check bounds on goto column
            if (goto_coords.column > NO_GOTO) {

                if ( goto_coords.column < 1
                     || goto_coords.column > col_count )
                {
                    msgout (DEBUG, "Ignoring out of bounds goto column of %d\n", goto_coords.column);
                    continue;
                }
            }


            // check bounds on goto row
            if (goto_coords.row > NO_GOTO) {

                if ( goto_coords.row < 1
                     || goto_coords.row > row_count )
                {
                    msgout (DEBUG, "Ignoring out of bounds goto row of %d\n", goto_coords.row);
                    continue;
                }
            }

            // we must destroy our current face and create another one
            // if the indicated face_type is different or the number
            // of faces has changed
            if (cfg->options->face_type != faces->get_type()
                || cfg->options->recalc)
            {
                //msgout(DEBUG, "switch faceset %d for %d\n", 
                //       faces->get_type(), cfg->options->face_type);

                face_set->reconfigure(row_count, col_count);
                set_arrangement (cfg->options->default_z_offset_distance);

                cfg->options->recalc = 0;
            }


            // reload face set if different number of columns or rows
            if (col_count != faces->get_number_of_columns() ||
                row_count != faces->get_number_of_rows() ) 
            {
                msgout (DEBUG, "++++ column_count changed from %d to %d\n", 
                        faces->get_number_of_columns(), col_count);

                face_set->reconfigure(row_count, col_count);
                set_arrangement (cfg->options->default_z_offset_distance);

            }

            // (re)load the background image if the one in the cfg is new
            if (cfg->use_background_image && background_image_is_new(cfg->default_background)) {
                load_background_image();
            }

            done = 1;
            break;
        default:
            msgout (ERROR, "Unknown message! mytpe = %d\n", msgmgr.msg.mtype);
            break;
        }
    } while (!done);

    SET_STATE(STATE_NORMAL);

    msgout (DEBUG, "Activated\n");
    
    return goto_coords;
}


#if 0
static void
idle_func (void)
{
    static int count = 0;

    // context every 30 calls
    if (++count >= 30) {
        msgout (DEBUG, "a;lskdjf;asldfjasldkjf\n");
        context_switch();
        count = 0;
    } 

    render_scene();
        
}
#endif


#if 1
static void 
idle_func(void)
{
    // This prevents the scene from being rendered more then 1000
    // times a second ie 1000fps -- by limiting it like this we sneak
    // in some context switches so the CPU usage isn't through the
    // roof.

    long start = get_milli_time();

    if ( (start - last_renderscene_time) >= 10 ) {

        //msgout (DEBUG, "diff = %lu\n", start - last_renderscene_time);

        render_scene();

    } else {
        //sleep_ms(1);  // allow context switch
        context_switch();
    }

} // END idle_func
#endif

void 
acquire_desktop (int rv)
{
    if (disable_autoacquire || !cfg->autoacquire)
        return;

    int current_row = 0, current_col = 0;
    int row_count = 0, col_count = 0;

    vdesks.get_vdesktop_info (&current_col, &current_row, &col_count, &row_count);

    msgout (DEBUG, ")))) GRABBING %d x %d\n", current_col, current_row);

    if (GLWin.grab_screenshot_data() < 0) {
        msgout (ERROR, "couldn't grab screen image\n");
        end_program(-1);
    }

    time_of_last_acquire = time(NULL);
    
    face_set->load_texture_data(current_col + (current_row * col_count), 
                                cfg->texture_size,
                                GLWin.get_screenshot_data_ptr() );

    faces->make_display_list();

    if (cfg->autoacquire)
        alarm(cfg->autoacquire);

} // END acquire_desktop


static int 
main_loop(void)
{
    XEvent event;
    KeySym key;
    Bool quit = False;

    desktop_coords_t  goto_coords;
    desktop_coords_t  current_count;
    desktop_coords_t  current_coords;

    while(!quit) {

        goto_coords.column = NO_GOTO;
        goto_coords.column = NO_GOTO;
        goto_on_flag = 0;

        goto_coords = wait_for_activation();

        current_coords.column = faces->get_current_column();
        current_coords.row = faces->get_current_row();
        current_count.column = faces->get_number_of_columns();
        current_count.row = faces->get_number_of_rows();
        
        // last coords are used when you press L or in alt_mousebuttons
        last_coords = current_coords;

#ifndef SCAN_MODE
        if (do_screenshot &&
            (time(NULL) - time_of_last_acquire) > 1) {
            
            if (GLWin.grab_screenshot_data() < 0) {
                msgout (ERROR, "couldn't grab screen image\n");
                end_program(-1);
            }

            face_set->load_texture_data(current_coords.column + (current_coords.row * current_count.column), 
                                        cfg->texture_size,
                                        GLWin.get_screenshot_data_ptr() );

            faces->make_display_list();
        } 
        //else {
        //    msgout(DEBUG, "~~~~~~~ NOT GRABBING\n");
        //}
#endif

        if (cfg->options->do_fullscreen)
            GLWin.unhide_GL_window();

        initiate_destroy = 0;

        if (goto_coords.column != NO_GOTO
            || goto_coords.row != NO_GOTO) 
        {
            setup_goto_face(goto_coords, 
                            current_coords,
                            current_count);
        }

        SET_STATE(STATE_ENTRY);
        em.trigger_event (ENTRY_MOVEMENT_START);


        end_X_event_loop = False;

        // finally this is so the "fun" behavior doesn't start right away
        last_keypress = get_milli_time();
    
        // wait for events
        while (!end_X_event_loop)
        {
            // handle the events in the queue 
            while (XPending(GLWin.get_display()) > 0)
            {
                XNextEvent(GLWin.get_display(), &event);
                switch (event.type)
                {
                case Expose:
                    if (event.xexpose.count != 0)
                        break;
                    render_scene();
                    break;
                case ConfigureNotify:
                    // call resizeGLScene only if our window-size changed 
                    if (((unsigned int)event.xconfigure.width != GLWin.get_width()) || 
                        ((unsigned int)event.xconfigure.height != GLWin.get_height()))
                    {
                        GLWin.resize_scene(event.xconfigure.width,
                                           event.xconfigure.height);
                    }
                    break;
                    // exit in case of a mouse button press 
                case ButtonPress:
                    mouse_pressed (event.xbutton.button);
                    break;
                case KeyPress:
                    key = XLookupKeysym(&event.xkey, 0);
                    key_pressed (key);
                    break;
                case ClientMessage:
                    if (*XGetAtomName(GLWin.get_display(), event.xclient.message_type) == 
                        *"WM_PROTOCOLS")
                    {
                        msgout(INFO, "Exiting sanely...\n");
                        end_X_event_loop = True;
                        quit = True;
                    }

                    break;
                default:
                    break;
                }
            }
            
            if (cfg->use_context_switch)
                idle_func();
            else
                render_scene();

        }

        if (cfg->options->do_fullscreen)
            GLWin.hide_GL_window();

    }


    return 0;

}  // END main_loop




static void
gl_init (void)
{
    // Color to clear color buffer to.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Depth to clear depth buffer to; type of test.
    glClearDepth(1.0);
    glDepthFunc(GL_LESS); 

    // Enables Smooth Color Shading; try GL_FLAT for (lack of) fun.
    glShadeModel(GL_SMOOTH);

    camera.PositionCamera(0, 0, 0,   // position
                          0, 0, 1,   // view
                          0, 1, 0 ); // up vector


    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, ones);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ones);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);

#if 0
    float spot_direction[] = { 0.0, 0.0, -1.0 };
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLighti(GL_LIGHT1, GL_SPOT_EXPONENT, 30);
    glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 180);
#endif

    if (cfg->options->use_breathing)
        breathing.init(&ambient_value, 1.7, 3.0, 200, Movement::MOVE_TYPE_SINE);
    else
        light_ambient[0] = light_ambient[1] = light_ambient[2] = 2.0;

    

} // END gl_init



static void 
load_digits(void) 
{
        
    char filename[100];
    unsigned char *data;
    unsigned int sizex = 256, sizey = 256;
    
    extern Config *cfg;

    // load digits for screens we don't have yet


    // allocate space for texture
    data = new unsigned char [sizex * sizey * 4];
    if (data == NULL) {
        msgout(ERROR, "Error allocating space for image");
        end_program(0);
    }
        
    // try loading /usr/share/3ddesktop/digits.bmp
    //             /usr/local/share/3ddesktop/digits.bmp
    //             ~/.3ddesktop/digits.bmp
    //             ./digits.bmp

    sprintf (filename, "%s/digits.bmp", cfg->options->base_dir);

    if (!load_image(filename, data, &sizex, &sizey, 1)) {

        sprintf (filename, SHAREDIR "/digits.bmp");

        if (!load_image(filename, data, &sizex, &sizey, 1)) {

            sprintf (filename, "%s/digits.bmp", cfg->working_dir);
            
            if (!load_image(filename, data, &sizex, &sizey, 1)) {
                
                sprintf (filename, "./digits.bmp");
                
                if (!load_image(filename, data, &sizex, &sizey, 1)) {
                    msgout (DEBUG, "no digits.bmp found\n");
                    cfg->options->show_digits = 0;
                    return ;
                }
            }
        }
    }
        
    // Create Texture
    glGenTextures(1, &digit_texture );
    glBindTexture(GL_TEXTURE_2D, digit_texture );   // 2d texture (x and y size)
        
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, MIN_FILTER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, MAX_FILTER);
        
    // 2d texture, 3 components (red, green, blue), x size from image, y size from image, 
    // rgb color data, unsigned byte data, and finally the data itself.
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, sizex, sizey, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //glTexImage2D(GL_TEXTURE_2D, 0, 3, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
    delete data;

} // END load_digits

#if defined(__FreeBSD__)
static char background_img_in_use[MAXNAMLEN + 1];
#else
static char background_img_in_use[PATH_MAX];
#endif


static int
background_image_is_new(char *bg_image_filename) 
{
    return strcmp(bg_image_filename, background_img_in_use);
}

static void 
load_background_image(void) 
{
        
    char filename[100];
    unsigned char *data;
    unsigned int bg_img_size = cfg->bg_texture_size;
    
    extern Config *cfg;

    // allocate space for texture
    data = new unsigned char [bg_img_size * bg_img_size * 4];
    if (data == NULL) {
        msgout(ERROR, "Error allocating space for image");
        end_program(0);
    }
        
    // try loading /usr/share/3ddesktop/x
    //             /usr/local/share/3ddesktop/x
    //             ~/.3ddesktop/x
    //             ./x

    Imlib_Image bg_img;

    sprintf (filename, "%s", cfg->default_background);

    if (! (bg_img = imlib_load_image(filename)) ) {

        sprintf (filename, SHAREDIR "/%s", cfg->default_background);

        if (! (bg_img = imlib_load_image(filename)) ) {

            sprintf (filename, "%s/%s", cfg->working_dir, 
                     cfg->default_background);
            
            if (! (bg_img = imlib_load_image(filename)) ) {
                
                sprintf (filename, "./%s", cfg->default_background);
                
                if (! (bg_img = imlib_load_image(filename)) ) {
                    msgout (DEBUG, "background image: %s not found\n", 
                            cfg->default_background);
                    cfg->use_background_image = 0;
                    return ;
                }
            }
        }
    }

    strcpy(background_img_in_use, cfg->default_background);

    convert_imlib_image_to_opengl_data(bg_img_size, bg_img, data);

    imlib_context_set_image(bg_img);
    imlib_free_image();

    // Create Texture
    glGenTextures(1, &background_texture );
    glBindTexture(GL_TEXTURE_2D, background_texture );   // 2d texture (x and y size)
        
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, MIN_FILTER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, MAX_FILTER);
        
    // 2d texture, 3 components (red, green, blue), x size from image, y size from image, 
    // rgb color data, unsigned byte data, and finally the data itself.
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bg_img_size, bg_img_size, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //glTexImage2D(GL_TEXTURE_2D, 0, 3, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
    delete data;

} // END load_background_image


// accesses global cfg
static void
initialize_3ddesktop (void)
{
    TIMEIT_INIT();

    // creates ~/.3ddesktop or /tmp/.3ddesktop-user if not alread there
    // used for pid file but no longer the screenshot
    //cfg->create_working_dir_if_necessary ();

    // there is a race between here and marking that I am running
    // but it should be small.  We must separate these because the
    // semaphore ownership is not inherited if we daemonize.
    if (check_if_i_am_running()) {
        msgout (ERROR, "3ddesktop is already running.  bye\n");
        exit (1);
    }
        
    if (vdesks.init (cfg) < 0) {
        msgout (ERROR, "Failed to initialize display.\n");
        exit(1);
    }

    int current_row = 0, current_col = 0;
    int row_count = 0, col_count = 0;

    vdesks.get_vdesktop_info (&current_col, &current_row, &col_count, &row_count);


#if 0
    if ((row_count * col_count) > MAX_FACES) {
        msgout (ERROR, "Can't support this many virtual desktops (%d), MAX is %d.\n", 
                row_count * col_count, MAX_FACES);
        end_program (1);
    }
#endif

#if 0
    if (row_count <= 1 && col_count <= 1) {
        msgout (ERROR, "You only have %d x %d virtual desktop(s)!  Need at least 2 to\n"
                "switch between them...\n", col_count, row_count);
        end_program (1);
    }
#endif

    // the actual acquire is done below but the message must be here,
    // before we daemonize
    if (cfg->options->acquire)
        msgout(INFO, 
               "\n"
               "================================================================\n"
               "3ddesktop will be acquiring images in one moment. Please wait...\n"
               "================================================================\n\n");


    // become a daemon 
    if (!cfg->verbose) {
        openlog ("3ddeskd", LOG_CONS, LOG_DAEMON);
        
        if (daemon_init() < 0) {
            msgout(ERROR, "Failed to daemonize.\n");
            end_program(-1);
        }

        cfg->options->daemonized = 1;
    }

    struct sigaction sa;
    sigset_t mask;

    sigemptyset (&mask);
    memset (&sa, 0, sizeof (struct sigaction));

    sa.sa_mask = mask;
    sa.sa_handler = end_program;

    sigaction (SIGTERM, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGQUIT, &sa, NULL);

    sa.sa_mask = mask;
    sa.sa_handler = acquire_desktop;
    sigaction (SIGALRM, &sa, NULL);

    if (cfg->autoacquire)
        alarm(cfg->autoacquire);

    // open the messaging mechanism (message queue) we will listen on
    
    if (msgmgr.open(MessageManager::CREATE) < 0) {
        msgout(ERROR, "Could not create message queue! %s\n", strerror(errno));
        end_program(-1);
    }

    if (cfg->make_pidfile() < 0)
        end_program(-1);


    // initialize opengl

    // our own init function.
    gl_init();

    GLWin.open_display();


    GLWin.create_GL_window("Enhanced 3-dimensional workspace visualization magic machine", 
                           cfg->options->do_fullscreen);

    cfg->texture_size = GLWin.get_best_texture_size(cfg->texture_size);

    // Loads up the correct perspective matrix
    GLWin.resize_scene();

    if (cfg->options->do_fullscreen)
        GLWin.hide_GL_window();

    // this must be here after opengl is initialized since the FaceSet
    // constructor loads the digit textures.
    face_set = new FaceSet(row_count, col_count);  // never deleted
    if (face_set == NULL) {
        msgout (ERROR, "Out of memory: face_set\n");
        end_program(-1);
    }

    last_time = get_milli_time();

    initialize_arrangements();

    set_arrangement(cfg->options->default_z_offset_distance);

    // loop through desktops and capture their images...  
    // ... or, if mirror, don't switch or capture desktops, just grab
    // current and set to all
    if (cfg->options->acquire || cfg->mirror) {

        if (!cfg->mirror)
            sleep(1);

        int firsttime = 1;
        int i, j;
        for (i = 0; i < row_count; i++) {

            for (j = 0; j < col_count; j++) {

                if (!cfg->mirror) {
                    vdesks.set_vdesktop (j, i);

                    if (cfg->options->acquire > 1) {
                        msgout(DEBUG, "sleeping %d...\n", cfg->options->acquire);
                        sleep_ms (cfg->options->acquire);
                    }
                }
                
                if (firsttime || !cfg->mirror) {
                    if (GLWin.grab_screenshot_data() < 0) {
                        msgout (ERROR, "couldn't grab screen image\n");
                        end_program(-1);
                    }
                    firsttime = 0;
                }
                
                face_set->load_texture_data(j + (i * col_count), 
                                            cfg->texture_size,
                                            GLWin.get_screenshot_data_ptr() );
            }
        }

        // go back to current
        if (!cfg->mirror)
            vdesks.set_vdesktop (current_col, current_row);

    }

    load_digits();

    if (cfg->use_background_image)
        load_background_image();

    if (flag_that_i_am_running()) {
        // (if we've daemonized this goes to syslog which is not easily seen)
        msgout(ERROR, "Oops, someone beat us to start up!!  Amazing!\n");
        end_program(-1);
    }

    em.add_event (ENTRY_MOVEMENT_START, entry_move_start_hook, NULL, 1);
    em.add_event (EXIT_MOVEMENT_START, exit_move_start_hook, NULL, 1);
                      
} //  END initialize_3ddesktop



int main(int argc,char **argv)
{
    cfg = new Config();   // deleted in end_program()
    if (!cfg) {
        msgout (ERROR, "new Config: out of memory\n");
        end_program(-1);
    }

    cfg->init (argc, argv);

    cfg->load_conf();

    initialize_3ddesktop();

    int rc = setpriority(PRIO_PROCESS, 0, cfg->priority);
    if (rc < 0) {
        msgout(DEBUG, "~~~~~ failed to set priority %d\n", cfg->priority);
    }

    return main_loop();

}  // END main
