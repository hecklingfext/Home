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
#include <string.h>   // for memset

using namespace std;
#include <stack>

#include "3ddesk.h"
#include "arrange.hpp"
#include "util.h"

#define LINEAR_MOVE_TYPE      Movement::MOVE_TYPE_WACK
#define CYLINDER_MOVE_TYPE    Movement::MOVE_TYPE_WACK
#define VIEWMASTER_MOVE_TYPE  Movement::MOVE_TYPE_WACK
#define CAROUSEL_MOVE_TYPE    Movement::MOVE_TYPE_WACK

// ==============================================================
//  Arrangement - abstract base class 
// ==============================================================

Arrangement::Arrangement (FaceSet *fs, Config *c)
{
    // defaults
    z_offset_distance = 7.0;
    face_list = glGenLists(1);
    move_type = Movement::MOVE_TYPE_WACK;

    cfg = c;
    face_set = fs;

    msgout(DEBUG, "Arrangement constructor\n");
    //memset(faces, 0, sizeof(faces));
}

Arrangement::~Arrangement ()
{
    glDeleteLists(face_list, 1);
}

void Arrangement::set_active (int i)
{
    if (face_set)
        face_set->set_active(i);
}

void Arrangement::set_current_face (int col, int row) 
{
    current_col = col;
    current_row = row;
    current_face = col + (row * n_columns);
}

int Arrangement::get_current_column(void) 
{
    return current_col;
}

int Arrangement::get_current_row(void) 
{
    return current_row;
}

int Arrangement::get_number_of_columns(void) 
{
    return n_columns;
}

int Arrangement::get_number_of_rows(void) 
{
    return n_rows;
}

float Arrangement::get_current_z_offset(void) 
{
    return z_offset;
}

void Arrangement::set_z_offset_distance(float zod) 
{
    z_offset_distance = zod;
}

int Arrangement::cant_exit_in_goto(void)
{
    // if not zooming the it is true that we cant exit while in goto
    return !cfg->options->entry_exit_movement;
}

void Arrangement::goto_up(void)
{
    goto_right();
}
void Arrangement::goto_down(void)
{
    goto_left();
}

void Arrangement::goto_right(void) 
{
    current_col++;
    if (current_col == n_columns)
        current_col = 0;   // cycle around

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void Arrangement::goto_left(void) 
{
    if (current_col == 0)
        current_col = n_columns - 1;  // cycle around
    else
        current_col--;
        
    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

// this function "unrolls" a face to col and row "coords" it does not
// check bounds
float Arrangement::goto_face (int face)
{
    int col = face % n_columns;
    int row = face / n_columns;

    return goto_face(col, row);
}

float Arrangement::goto_face (int col, int row) 
{
    desktop_coords_t  g;

    g.column = col;
    g.row = row;
    
    return goto_face(g);
}

// this goto face assumes a circular arrangement
// overload for other
float Arrangement::goto_face (desktop_coords_t dest) 
{
    if (dest.column < 0 
        || dest.column >= n_columns
        || dest.row < 0
        || dest.row >= n_rows)
    {
        //msgout (DEBUG, "row / col goto out of range (%u x %u)\n", 
        //        dest.column, dest.row);
        return 0;
    }

    int newface = dest.column + (dest.row * n_columns);

    int current = current_col + (current_row * n_columns);

    int number = n_columns * n_rows;

    float new_rot;

    // eeek!  figure out the shortest distance to the new face... there
    // must be a more elegant way!  but for now this works...
    if (newface > current) {
        if ( (newface - current) <
             ((current + number) - newface) )
        { 
            // normal case 
            new_rot = -per_face_change * (newface - current);
        } else {
            // "wrapped"
            new_rot =  per_face_change * ((current + number) - newface);
        }
    } else {
        // newface < current

        if ( (current - newface) >
             ((newface + number) - current) )
        {
            // "wrapped"
            new_rot = -per_face_change * ((newface + number) - current);
        } else {
            // normal case
            new_rot =  per_face_change * (current - newface);
        }
    }

    if (goto_face_mv.at_destination())
        goto_face_mv.init (&goto_face_val, goto_face_val, 
                           goto_face_val + new_rot,
                           cfg->options->face_change_steps, move_type);
    else
        goto_face_mv.init (&goto_face_val, goto_face_val, 
                           goto_face_mv.destination() + new_rot,
                           cfg->options->face_change_steps, move_type);

    current_col = dest.column; //newface; 
    current_row = dest.row;
    current_face = newface;
    
    return cfg->options->face_change_steps;
}

int Arrangement::in_goto (void)
{
    return !goto_face_mv.at_destination();
}


void Arrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));

    goto_face (which_one);
}


void Arrangement::entry_movement_start(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    z_off_mv.init (&z_offset, 
                   z_offset_close, get_distant_z_offset(), 
                   cfg->options->zoom_steps, 
                   Movement::MOVE_TYPE_SMOOTH);
}

void Arrangement::exit_movement_start(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    z_off_mv.init(&z_offset, 
                  z_offset, z_offset_close, 
                  cfg->options->zoom_steps, Movement::MOVE_TYPE_WACK);
}

void Arrangement::exit_movement_cancel(void)
{
    if (!cfg->options->entry_exit_movement)
        return;

    z_off_mv.init (&z_offset,
                   z_offset, get_distant_z_offset(),
                   cfg->options->zoom_steps,
                   Movement::MOVE_TYPE_SMOOTH);
}

int Arrangement::is_entry_movement_done(void) 
{
    return (!cfg->options->entry_exit_movement || z_off_mv.at_destination());
}

int Arrangement::is_exit_movement_done(void) 
{
    return (!cfg->options->entry_exit_movement || z_off_mv.at_destination());
}

void Arrangement::entry_exit_move(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    z_off_mv.change_a_bit();
}

void Arrangement::set_transparency (float t)
{
    face_set->set_transparency (t);
}

void Arrangement::render()
{
    face_set->render_all(current_face);
    //face_set->render_all();
}

// sacrifice the call list for the pretty transparency... :/

//void Arrangement::render()
//{
//    glCallList(face_list);
//}

void Arrangement::make_display_list(void)
{
    glMatrixMode(GL_MODELVIEW);

    glNewList(face_list,GL_COMPILE);

    face_set->render_all();

    glEndList();
}

void Arrangement::position_lighting(void)
{
    // For most arrangements (carousel, cylinder, priceisright,
    // viewmaster) we want to move the camera out away from the faces
    // but not rotate around to the current face (goto_face_val) as
    // that would spin the lighting around.
    camera->Look();
}


int Arrangement::allow_blending(void)
{
    return true;
}

// ==============================================================
//  CarouselArrangement
// ==============================================================

CarouselArrangement::CarouselArrangement(VDesktops &vdesks, 
					 float fw, float fh, 
					 FaceSet *fs,
					 Config *c,
                                         Camera *cam)
					 
    : Arrangement (fs, c)
{
    X_Rot = 0;
    x_rot_final = 15.0;

    face_width = fw;
    face_height = fh;

    camera = cam;

    calculate_faces(vdesks);

    goto_face_mv.disable();
}

void CarouselArrangement::calculate_faces(VDesktops & vdesks)
{

    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);
    
    // current_face becomes the unrolled current face
    current_face = current_col + (current_row * n_columns);
    
    // n_faces becomes the total number of desktops 
    n_faces = n_columns * n_rows;
    
    side_angle = 360.0 / (float)n_faces;
    
    per_face_change = side_angle;
    
    goto_face_val = 360 - (side_angle * current_face);
    
    // make this value up - give us space between each face
    float circum = face_width * 1.5 * n_faces;
    
    // this is a class value that we keep
    radius = circum / (2.0 * PI);

    // angle in radians of right triangle angle from camera to face
    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    float half_face_height = face_height / 2.0;
    float half_face_width = face_width / 2.0;

    // find distance from camera to face to show whole face then add
    // radius to it. get_base_z_offset returns radius
    z_offset_close = z_offset = -(float)(half_face_height / tan(my_angle) + 
                                         get_base_z_offset());
    

//    msgout (DEBUG, "_________ rad %f  my_angle %f  circum %f  z_off %f  z_off_c %f  fw %f  fh %f\n", 
//            radius, my_angle, circum, z_offset, z_offset_close, 
//            face_width, face_height);

    int i;

    Point tl,tr,bl,br;

    for (i = 0; i < n_faces; i++) {
        
        tl.set (-half_face_width,  half_face_height, 0);
        tr.set ( half_face_width,  half_face_height, 0);
        bl.set (-half_face_width, -half_face_height, 0);
        br.set ( half_face_width, -half_face_height, 0);

        translate (tl.v, 0, 0, radius);
        rotate_y (tl.v, side_angle * i);

        translate (tr.v, 0, 0, radius);
        rotate_y (tr.v, side_angle * i);

        translate (bl.v, 0, 0, radius);
        rotate_y (bl.v, side_angle * i);

        translate (br.v, 0, 0, radius);
        rotate_y (br.v, side_angle * i);

//        msgout (DEBUG, "[%i] = %f, %f, %f\n", i, tl.v[0], tl.v[1], tl.v[2]);


        // the palindromic serendipity of this rocks!
        face_set->set_face(i, tl, tr, bl, br);

#if 0
        msgout(DEBUG, "[%i] X tl=%f, tr=%f, br=%f\n", i, 
                face_set->topleft.v[0], face_set->topright.v[0], face_set->bottomright.v[0]);
        msgout(DEBUG, "[%i] Y tl=%f, tr=%f, br=%f\n", i, 
                face_set->topleft.v[1], face_set->topright.v[1], face_set->bottomright.v[1]);
        msgout(DEBUG, "[%i] Z tl=%f, tr=%f, br=%f\n", i, 
                face_set->topleft.v[2], face_set->topright.v[2], face_set->bottomright.v[2]);

        msgout(DEBUG, "[%i] normal = %f, %f, %f\n\n", i, 
                f->normal.v[0], f->normal.v[1], f->normal.v[2]);
#endif

    }

    //make_display_list();
}

int CarouselArrangement::get_type () 
{
    return FACE_TYPE_CAROUSEL;
}

void CarouselArrangement::set_current_face (int c, int r) 
{
    current_face = c + (r * n_columns);
    current_col = c;
    current_row = r;
    goto_face_val = 360 - (side_angle * current_face);
    goto_face_mv.disable();
}

float CarouselArrangement::get_distant_z_offset(void) 
{
    return -(z_offset_distance + get_base_z_offset());
}

float CarouselArrangement::get_base_z_offset(void) 
{
    return (radius);
}


void CarouselArrangement::do_movement(void) 
{
    if (!goto_face_mv.at_destination())
        goto_face_mv.change_a_bit ();
    else 
        clamp_degrees(&goto_face_val);

}

void CarouselArrangement::position_faces(void) 
{
    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;

    //glRotatef(X_Rot, 1.0f, 0.0f, 0.0f);
    extern Vector zero;  // rotate around the center (0,0,0)
    camera->RotateAroundPoint(zero, -deg_to_rad(X_Rot), 1, 0, 0);

    camera->Look();

    glRotatef(goto_face_val, 0.0f, 1.0f, 0.0f);
    //camera->RotateAroundPoint(center, -deg_to_rad(goto_face_val), 0, 1, 0);
}

void CarouselArrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));
    
    current_face = which_one;

    goto_face (which_one);
}

void CarouselArrangement::goto_right(void) 
{
    current_face++;
    if (current_face == n_faces)
        current_face = 0;   // cycle around

    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void CarouselArrangement::goto_left(void) 
{
    if (current_face == 0)
        current_face = n_faces - 1;  // cycle around
    else
        current_face--;
        
    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void CarouselArrangement::entry_movement_start(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    x_rot_mv.init (&X_Rot,
                   0.0, x_rot_final,
                   cfg->options->zoom_steps,
                   Movement::MOVE_TYPE_SMOOTH);
    Arrangement::entry_movement_start();
}

void CarouselArrangement::exit_movement_start(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    x_rot_mv.init(&X_Rot,
                  X_Rot, 0.0,
                  cfg->options->zoom_steps,
                  Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_start();
}

void CarouselArrangement::exit_movement_cancel(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    x_rot_mv.init(&X_Rot,
                  X_Rot, x_rot_final,
                  cfg->options->zoom_steps,
                  Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_cancel();
}

void CarouselArrangement::entry_exit_move(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    x_rot_mv.change_a_bit();
    Arrangement::entry_exit_move();
}


#if !defined (ONE_ARRANGEMENT_ONLY)


// ==============================================================
//  CylinderArrangement
// ==============================================================


CylinderArrangement::CylinderArrangement(VDesktops &vdesks, 
					 float fw, float fh, 
					 FaceSet *fs,
					 Config *c,
                                         Camera *cam)
    : Arrangement (fs, c)
{
    two_face_gap = 3;

    z_offset_distance = 5.0;

    X_Rot = 0.0;
    x_rot_final = 15.0;

    face_width = fw;
    face_height = fh;
    
    camera = cam;

    calculate_faces(vdesks);
    
    goto_face_mv.disable();
}

void CylinderArrangement::calculate_faces(VDesktops & vdesks) 
{
    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);
    
    // current_face becomes the unrolled current face
    current_face = current_col + (current_row * n_columns);

    // n_faces becomes the total number of desktops 
    n_faces = n_columns * n_rows;
                
    side_angle = 360.0 / (float)n_faces;
    per_face_change = side_angle;

    goto_face_val = 360 - (side_angle * current_face);


    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    // get_base_z_offset returns radius
    z_offset_close = z_offset = -(float)((face_height/2.0) / tan(my_angle) + 
                                         get_base_z_offset());
    
    int i;
    float half_face_height = face_height / 2.0;
    float half_face_width = face_width / 2.0;

    Point tl,tr,bl,br;
    
    if (n_faces == 2) {

        // 2 faces really messes up becase the faces sandwhich
        // together and you can't see one of the textures so special
        // case it and move them apart

        tl.set (-half_face_width,  half_face_height, two_face_gap);
        tr.set ( half_face_width,  half_face_height, two_face_gap);
        bl.set (-half_face_width, -half_face_height, two_face_gap);
        br.set ( half_face_width, -half_face_height, two_face_gap);

        face_set->set_face (0, tl, tr, bl, br);

        tl.set ( half_face_width,  half_face_height, -two_face_gap);
        tr.set (-half_face_width,  half_face_height, -two_face_gap);
        bl.set ( half_face_width, -half_face_height, -two_face_gap);
        br.set (-half_face_width, -half_face_height, -two_face_gap);

        face_set->set_face (1, tl, tr, bl, br);

    } else {
        polypoints_t pp;

        calculate_polypoints (n_faces, face_width, &pp);
        for (i = 0; i < n_faces; i++) {
            // translate the values calculated for a n-face polygon in
            // a x-y coord system to the 3d x-y-z coords.  we don't
            // need y because the up and down (y-axis) is always
            // either half_face_height or -half_face_height
            
            //msgout(DEBUG, "%u:: x=%lf    y=%lf\n", i, pp.x[i], pp.y[i]);
            
            tl.set( pp.x[i],  half_face_height, -pp.y[i]);
            bl.set( pp.x[i], -half_face_height, -pp.y[i]);
            
            if (i == n_faces - 1) {
                tr.set( pp.x[0],  half_face_height, -pp.y[0]);
                br.set( pp.x[0], -half_face_height, -pp.y[0]);
            } else {
                tr.set( pp.x[i+1],  half_face_height, -pp.y[i+1]);
                br.set( pp.x[i+1], -half_face_height, -pp.y[i+1]);
            }
            
            face_set->set_face (i, tl, tr, bl, br);

        }
    }

    //make_display_list();

}

int CylinderArrangement::get_type () 
{
    return FACE_TYPE_CYLINDER;
}

void CylinderArrangement::set_current_face (int c, int r) 
{
    current_col = c;
    current_row = r;
    current_face = c + (r * n_columns);
    goto_face_val = 360 - (side_angle * current_face);
    goto_face_mv.disable();
}

float CylinderArrangement::get_distant_z_offset(void) 
{
    return -(z_offset_distance + get_base_z_offset());
}

float CylinderArrangement::get_base_z_offset(void) 
{
    if (n_faces == 2) {
        return two_face_gap;
    } else {
        return ((face_width/2.0) / tan (deg_to_rad(side_angle/2.0)));
    }
}

// this has to by overridden because of the 'current_face' variable
// which is private to this class
void CylinderArrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));
    
    current_face = which_one;

    goto_face (which_one);
}

void CylinderArrangement::goto_right(void) 
{
    current_face++;
    if (current_face == n_faces)
        current_face = 0;   // cycle around

    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void CylinderArrangement::goto_left(void) 
{
    if (current_face == 0)
        current_face = n_faces - 1;  // cycle around
    else
        current_face--;
        
    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}


void CylinderArrangement::do_movement(void) 
{
    if (!goto_face_mv.at_destination())
        goto_face_mv.change_a_bit ();
    else
        clamp_degrees(&goto_face_val);
}

void CylinderArrangement::position_faces(void) 
{
    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;

    //glRotatef(X_Rot, 1.0f, 0.0f, 0.0f);
    extern Vector zero;  // rotate around the center (0,0,0)
    camera->RotateAroundPoint(zero, -deg_to_rad(X_Rot), 1, 0, 0);

    camera->Look();

    glRotatef(goto_face_val, 0.0f, 1.0f, 0.0f);
}

void CylinderArrangement::entry_movement_start(void) 
{
    //if (!cfg->options->entry_exit_movement)
    //    return;

    x_rot_mv.init (&X_Rot,
                   0.0, x_rot_final,
                   cfg->options->zoom_steps,
                   Movement::MOVE_TYPE_SMOOTH);
    Arrangement::entry_movement_start();
}

void CylinderArrangement::exit_movement_start(void) 
{
    //if (!cfg->options->entry_exit_movement)
    //    return;

    x_rot_mv.init(&X_Rot,
                  X_Rot, 0.0,
                  cfg->options->zoom_steps,
                  Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_start();
}

void CylinderArrangement::exit_movement_cancel(void) 
{
    //if (!cfg->options->entry_exit_movement)
    //    return;

    x_rot_mv.init(&X_Rot,
                  X_Rot, x_rot_final,
                  cfg->options->zoom_steps,
                  Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_cancel();
}

void CylinderArrangement::entry_exit_move(void) 
{
    if (!cfg->options->entry_exit_movement)
        return;

    x_rot_mv.change_a_bit();
    Arrangement::entry_exit_move();
}




// ==============================================================
//  ViewMasterArrangement
// ==============================================================

ViewMasterArrangement::ViewMasterArrangement(VDesktops &vdesks, 
                                             float fw, float fh,
                                             FaceSet *fs,
                                             Config *c,
                                             Camera *cam)
    : Arrangement (fs, c)
{
    z_offset_distance = 10.0;
        
    face_width = fw;
    face_height = fh;

    camera = cam;

    calculate_faces(vdesks);

    goto_face_mv.disable();
}

void ViewMasterArrangement::calculate_faces(VDesktops & vdesks)
{
    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);
    
    // current_face becomes the unrolled current face
    current_face = current_col + (current_row * n_columns);

    // n_faces becomes the total number of desktops 
    n_faces = n_columns * n_rows;


    between_angle = 360 / n_faces;
    per_face_change = between_angle;

    center_distance = n_faces;  // distance from the center

    if (n_faces <= 3)
        center_distance = 3.5;

    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    z_offset_close = z_offset = -(float)((face_height/2.0) / tan(my_angle) + 
                                         get_base_z_offset());

    Point tl,tr,bl,br;
    int i;
    float angle;
    float center_x, center_y;
    float x1, y1, x2, y2, x3, y3, x4, y4;

    float r = sqrt ((face_height*face_height) + 
                    (face_width*face_width)) / 2.0;
    
    float angle_l, angle_s, hangle_l;

    for (i = 0; i < n_faces; i++) {
        angle = between_angle * i;

        center_x = polar_to_x (center_distance, angle);
        center_y = polar_to_y (center_distance, angle);

        hangle_l = rad_to_deg(atan(face_width/face_height));
        angle_l = hangle_l*2.0;
        angle_s = 180 - angle_l;
        

        x1 = polar_to_x(r, 90+angle+hangle_l) + center_x;
        y1 = polar_to_y(r, 90+angle+hangle_l) + center_y;
        x2 = polar_to_x(r, 90+angle-hangle_l) + center_x;
        y2 = polar_to_y(r, 90+angle-hangle_l) + center_y;
        x3 = polar_to_x(r, 90+angle+hangle_l+angle_s) + center_x;
        y3 = polar_to_y(r, 90+angle+hangle_l+angle_s) + center_y;
        x4 = polar_to_x(r, 90+angle+hangle_l+angle_s+angle_l) + center_x;
        y4 = polar_to_y(r, 90+angle+hangle_l+angle_s+angle_l) + center_y;

        tl.set(x1, y1, 0);
        tr.set(x2, y2, 0);
        bl.set(x3, y3, 0);
        br.set(x4, y4, 0);

#if 0
        if (i == 0) {
            msgout(DEBUG, "hangle_l=%f\n", hangle_l);
            msgout(DEBUG, "cx=%f  cy=%f\n", center_x, center_y);
            msgout(DEBUG, "r=%f\n", r);
            msgout(DEBUG, "x=%f  y=%f\n", x1, y1);
            msgout(DEBUG, "x=%f  y=%f\n", x2, y2);
            msgout(DEBUG, "x=%f  y=%f\n", x3, y3);
            msgout(DEBUG, "x=%f  y=%f\n", x4, y4);
        }
#endif

        face_set->set_face (i, tl, tr, bl, br);

    }

    goto_face_val = -current_face * between_angle;

    make_display_list();
}


int ViewMasterArrangement::get_type () 
{
    return FACE_TYPE_VIEWMASTER;
}


// this has to by overridden because of the 'current_face' variable
// which is private to this class
void ViewMasterArrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));
    
    current_face = which_one;

    goto_face (which_one);
}

void ViewMasterArrangement::goto_right(void) 
{
    current_face++;
    if (current_face == n_faces)
        current_face = 0;   // cycle around

    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void ViewMasterArrangement::goto_left(void) 
{
    if (current_face == 0)
        current_face = n_faces - 1;  // cycle around
    else
        current_face--;
        
    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}



void ViewMasterArrangement::set_current_face (int c, int r) 
{
    current_col = c;
    current_row = r;
    current_face = c + (r * n_columns);
    goto_face_val = -current_face * between_angle;
    goto_face_mv.disable();
}

float ViewMasterArrangement::get_distant_z_offset(void)
{
    return -(z_offset_distance);
}


float ViewMasterArrangement::get_base_z_offset(void)
{ 
    return 0.0;
}


void ViewMasterArrangement::position_faces(void) 
{
    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;
    
    //glTranslatef(-center_distance, 0.0f, 0.0f);
    camera->position.x = center_distance;
    camera->viewvector.x = center_distance;

    camera->Look();

    glRotatef(goto_face_val, 0.0f, 0.0f, 1.0f);
}

void ViewMasterArrangement::render()
{
    //glCallList(face_list);
    face_set->render_all();
}

void ViewMasterArrangement::do_movement(void) 
{
    if (!goto_face_mv.at_destination())
        goto_face_mv.change_a_bit();
    else
        clamp_degrees (&goto_face_val);
}


// ==============================================================
//  PriceIsRightArrangement
// ==============================================================
PriceIsRightArrangement::PriceIsRightArrangement(VDesktops &vdesks, 
                                                 float fw, float fh, 
                                                 FaceSet *fs,
                                                 Config *c, 
                                                 Camera *cam)
    : Arrangement (fs, c)
{
    face_width = fw;
    face_height = fh;
                
    camera = cam;

    calculate_faces(vdesks);
       
    goto_face_mv.disable();

}

void PriceIsRightArrangement::calculate_faces(VDesktops & vdesks)
{
    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);
    
    // current_face becomes the unrolled current face
    current_face = current_col + (current_row * n_columns);

    // n_faces becomes the total number of desktops 
    n_faces = n_columns * n_rows;

    side_angle = 360.0 / (float)n_faces;
    per_face_change = -side_angle;

    goto_face_val = 360 + (side_angle * current_face);


    float circum = face_height * 1.8 * n_faces;
    radius = circum / (2.0 * PI);



    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    z_offset_close = z_offset = -(float)((face_height/2.0) / tan(my_angle) + 
                                         get_base_z_offset());
    

    int i;
    float half_face_height = face_height / 2.0;
    float half_face_width = face_width / 2.0;

    Point tl,tr,bl,br;
    
    for (i = 0; i < n_faces; i++) {
        
        tl.set (-half_face_width,  half_face_height, 0);
        tr.set ( half_face_width,  half_face_height, 0);
        bl.set (-half_face_width, -half_face_height, 0);
        br.set ( half_face_width, -half_face_height, 0);

        translate (tl.v, 0, 0, radius);
        rotate_x (tl.v, -side_angle * i);

        translate (tr.v, 0, 0, radius);
        rotate_x (tr.v, -side_angle * i);

        translate (bl.v, 0, 0, radius);
        rotate_x (bl.v, -side_angle * i);

        translate (br.v, 0, 0, radius);
        rotate_x (br.v, -side_angle * i);

        //msgout(DEBUG, "[%i] = %f, %f, %f\n", i, tl.v[0], tl.v[1], tl.v[2]);

        face_set->set_face (i, tl, tr, bl, br);
    }

    //make_display_list();

}


int PriceIsRightArrangement::get_type () 
{
    return FACE_TYPE_PRICEISRIGHT;
}

// this has to by overridden because of the 'current_face' variable
// which is private to this class
void PriceIsRightArrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));
    
    current_face = which_one;

    goto_face (which_one);
}


void PriceIsRightArrangement::goto_right(void) 
{
    current_face++;
    if (current_face == n_faces)
        current_face = 0;   // cycle around

    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}

void PriceIsRightArrangement::goto_left(void) 
{
    if (current_face == 0)
        current_face = n_faces - 1;  // cycle around
    else
        current_face--;
        
    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }
}


void PriceIsRightArrangement::set_current_face (int c, int r) 
{
    current_col = c;
    current_row = r;
    current_face = c + (r * n_columns);
    goto_face_val = 360 + (side_angle * current_face);
    goto_face_mv.disable();
}

float PriceIsRightArrangement::get_distant_z_offset(void) 
{
    return -(z_offset_distance + get_base_z_offset());
}

float PriceIsRightArrangement::get_base_z_offset(void) 
{
    return (radius ); //(face_width/2.0) / tan (deg_to_rad(side_angle/2.0)));
}


void PriceIsRightArrangement::do_movement(void) 
{
    if (!goto_face_mv.at_destination())
        goto_face_mv.change_a_bit ();
    else
        clamp_degrees(&goto_face_val);
}

void PriceIsRightArrangement::position_faces(void) 
{
    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;

    camera->Look();

    glRotatef(goto_face_val, 1.0f, 0.0f, 0.0f);
}



// ==============================================================
//  LinearArrangement
// ==============================================================

LinearArrangement::LinearArrangement(VDesktops &vdesks, 
                                     float fw, float fh,
                                     FaceSet *fs,
                                     Config *c,
                                     Camera *cam)
    : Arrangement (fs, c)
{
    z_offset_distance = 10.0;

    spin_z_rot = 0;

    //cfg->options->zoom_steps = 60;

    face_width = fw;
    face_height = fh;

    camera = cam;

    calculate_faces(vdesks);

    x_trans_mv.disable();
    y_trans_mv.disable();
    spin_mv.disable();
}

void LinearArrangement::calculate_faces(VDesktops & vdesks)
{

    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);

    between = cfg->options->linear_spacing;

    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    z_offset_close = z_offset = -(float)((face_height/2.0) / tan(my_angle) + 
                                         get_base_z_offset());
    
    //msgout(DEBUG, "z_offset_close = %lf\n", z_offset_close);
    

    Point tl,tr,bl,br;
    int i, j, count = 0;
    float x, y;
    float half_face_height = face_height / 2.0;
    float half_face_width = face_width / 2.0;

    X_Trans = -(half_face_width + (current_col * (face_width + between)));
    Y_Trans = ((current_row * (face_height + between)) + half_face_height);

    //msgout (DEBUG, "constructor: Y_Trans = %f\n", Y_Trans);

    for (i = 0; i < n_rows; i++) {
        for (j = 0; j < n_columns; j++) {
            
            y = i * (face_height + between);
            x = j * (face_width + between);

            tl.set(x,              -y,               0);
            tr.set(x + face_width, -y,               0);
            bl.set(x,              -y - face_height, 0);
            br.set(x + face_width, -y - face_height, 0);

            face_set->set_face (count, tl, tr, bl, br);

            count ++;
        }
    }

    make_display_list();

}

int LinearArrangement::get_type () 
{
    return FACE_TYPE_LINEAR;
}


void LinearArrangement::set_current_face (int c, int r) 
{
    float half_face_width = face_width / 2.0;
    float half_face_height = face_height / 2.0;

    current_row = r;
    current_col = c;
    current_face = c + (r * n_columns);

    X_Trans = -(half_face_width + (current_col * (face_width + between)));
    Y_Trans = ((current_row * (face_height + between)) + half_face_height);

    msgout (DEBUG, "set_current_face: Y_Trans = %f, "
            "current_col=%d, current_row=%d, face_height=%f, "
            "between=%f, hfh=%f\n", Y_Trans, current_col, current_row,
            face_height, between, half_face_height);

    x_trans_mv.disable();
    y_trans_mv.disable();
}

float LinearArrangement::get_distant_z_offset(void)
{
    return -(z_offset_distance);
}


float LinearArrangement::get_base_z_offset(void)
{ 
    return 0.0;
}

int LinearArrangement::in_goto (void)
{
    return !x_trans_mv.at_destination() || !y_trans_mv.at_destination();
}

void LinearArrangement::goto_left(void) 
{
    if (current_col != 0)
        current_col--;
    else 
        return;

    if (x_trans_mv.at_destination()) {
        //if ((X_Trans + (face_width+between)) > MIN_X_Trans)   // because its negative
        //    break;
        x_trans_mv.init (&X_Trans, X_Trans, X_Trans + (face_width+between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        //if ((x_trans_mv.destination() + (face_width+between)) > MIN_X_Trans)   // because its negative
        //    break;
        x_trans_mv.init (&X_Trans, X_Trans, x_trans_mv.destination() + (face_width+between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
}

void LinearArrangement::goto_right(void) 
{
    if (current_col != (n_columns - 1))
        current_col++;
    else
        return;
    
    if (x_trans_mv.at_destination()) {
        x_trans_mv.init (&X_Trans, X_Trans, X_Trans - (face_width + between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        x_trans_mv.init (&X_Trans, X_Trans, 
                         x_trans_mv.destination() - (face_width+between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
}

void LinearArrangement::goto_up(void)
{
    if (current_row != 0)
        current_row--;
    else 
        return;

    if (y_trans_mv.at_destination()) {
        y_trans_mv.init (&Y_Trans, Y_Trans, Y_Trans - (face_height+between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        y_trans_mv.init (&Y_Trans, Y_Trans, y_trans_mv.destination() - (face_height+between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
}

void LinearArrangement::goto_down(void)
{
    if (current_row != (n_rows - 1))
        current_row++;
    else
        return;
    
    if (y_trans_mv.at_destination()) {
        y_trans_mv.init (&Y_Trans, Y_Trans, Y_Trans + (face_height + between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        y_trans_mv.init (&Y_Trans, Y_Trans, 
                         y_trans_mv.destination() + (face_height + between), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
}

float LinearArrangement::goto_face (desktop_coords_t dest)
{
    signed int nummove;
    
    if (//dest.column == current_col ||
        dest.column < 0
        || dest.column >= n_columns)
        return 0;
    
    nummove = dest.column - current_col;
    current_col = dest.column; 
    
    if (x_trans_mv.at_destination()) {
        x_trans_mv.init (&X_Trans, X_Trans, 
                         X_Trans - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        x_trans_mv.init (&X_Trans, X_Trans, 
                         x_trans_mv.destination() - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }


    if (//dest.row == current_row ||
        dest.row < 0
        || dest.row >= n_rows)
        return 0;
    
    nummove = dest.row - current_row;
    current_row = dest.row; 
    
    if (y_trans_mv.at_destination()) {
        y_trans_mv.init (&Y_Trans, Y_Trans, 
                         Y_Trans + ((float)nummove * (face_height + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        y_trans_mv.init (&Y_Trans, Y_Trans, 
                         y_trans_mv.destination() + ((float)nummove * (face_height + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
    return cfg->options->face_change_steps;

    
}


#if 0
float LinearArrangement::goto_column(int column) 
{
    signed int nummove;
    
    if (column == current_col
        || column < 0
        || column >= n_columns)
        return 0;
    
    nummove = column - current_col;
    current_col = column; 
    
    if (x_trans_mv.at_destination()) {
        x_trans_mv.init (&X_Trans, X_Trans, 
                         X_Trans - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        x_trans_mv.init (&X_Trans, X_Trans, 
                         x_trans_mv.destination() - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
    return cfg->options->face_change_steps;
}


float LinearArrangement::goto_row(int row) 
{
    signed int nummove;
    
    if (row == current_row
        || row < 0
        || row >= n_rows)
        return 0;
    
    nummove = row - current_row;
    current_row = row; 
    
    if (y_trans_mv.at_destination()) {
        y_trans_mv.init (&Y_Trans, Y_Trans, 
                         Y_Trans - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    } else {
        y_trans_mv.init (&Y_Trans, Y_Trans, 
                         y_trans_mv.destination() - ((float)nummove * (face_width + between)), 
                         cfg->options->face_change_steps, LINEAR_MOVE_TYPE);
    }
    return cfg->options->face_change_steps;
}
#endif // if 0


void LinearArrangement::position_faces(void) 
{
    //if (spin_z_rot)
    //    glRotatef(spin_z_rot,0.0f,0.0f,1.0f);

    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;
    
    //glTranslatef(X_Trans, 0.0f, 0.0f);
    camera->position.x = -X_Trans;
    camera->viewvector.x = -X_Trans;

    //glTranslatef(0.0f, Y_Trans, 0.0f);
    camera->position.y = -Y_Trans;
    camera->viewvector.y = -Y_Trans;

    camera->Look();
}

void LinearArrangement::position_lighting(void)
{
    // don't do anything as this will keep the light center on our
    // current view
}

void LinearArrangement::render()
{
    //glCallList(face_list);
    face_set->render_all();
}


void LinearArrangement::do_movement(void) 
{
    if (!x_trans_mv.at_destination()) {
        x_trans_mv.change_a_bit();
    }
    if (!y_trans_mv.at_destination()) {
        y_trans_mv.change_a_bit();
    }
}

void LinearArrangement::entry_exit_move(void) 
{
    //spin_mv.change_a_bit();
    Arrangement::entry_exit_move();
}

void LinearArrangement::exit_movement_start(void) 
{
    //spin_mv.init (&spin_z_rot, 
    //              0.0, 360.0, 
    //              cfg->options->zoom_steps, 
    //              Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_start();
}

void LinearArrangement::exit_movement_cancel(void) 
{
    //spin_mv.init (&spin_z_rot, 
    //              spin_z_rot, 0.0, 
    //              cfg->options->zoom_steps, 
    //              Movement::MOVE_TYPE_WACK);
    Arrangement::exit_movement_cancel();
}



// ==============================================================
//  FlipArrangement
// ==============================================================

// override so as to prevent "spin" from getting short circuited
int FlipArrangement::in_goto (void)
{
    return !goto_face_mv.at_destination() || !spin_mv.at_destination();
}

int FlipArrangement::cant_exit_in_goto(void)
{
    return 1;  // this is always true
}

// Faces in FaceSet aren't rendered at all, we render our own
void FlipArrangement::render()
{
    face_A->render();
    face_B->render();
}

// display list not utilized for this one
void FlipArrangement::make_display_list(void)
{ }


FlipArrangement::FlipArrangement(VDesktops & vdesks, 
                                 float fw, float fh,
                                 FaceSet *fs,
                                 Config *c,
                                 Camera *cam)
    : Arrangement (fs, c)
{
    face_width = fw;
    face_height = fh;

    camera = cam;

    face_A = new Face (0);
    if (!face_A) {
        msgout(ERROR, "out of memory\n");
        exit (-1);
    }

    face_B = new Face (0);
    if (!face_B) {
        msgout(ERROR, "out of memory\n");
        exit (-1);
    }

    showing_face = NULL;

    calculate_faces(vdesks);

    goto_face_val = 0;

    goto_face_mv.disable();

    spin_mv.disable();
    spin_val = 0;
}

FlipArrangement::~FlipArrangement () 
{
    delete face_A;
    delete face_B;
}

int FlipArrangement::get_type()
{
    return FACE_TYPE_FLIP;
}

void FlipArrangement::calculate_faces(VDesktops & vdesks)
{
    vdesks.get_vdesktop_info (&current_col, &current_row,
                              &n_columns, &n_rows);
    
    //msgout (DEBUG, "at %d x %d (%d, %d)\n", current_row, current_col, n_rows, n_columns);

    // current_face becomes the unrolled current face
    current_face = current_col + (current_row * n_columns);

    // n_faces becomes the total number of desktops 
    n_faces = n_columns * n_rows;

    z_offset_distance = 10.0;

    per_face_change = 180;

                
    float my_angle = ((FOV / 2.0) * PI) / 180.0;
    
    z_offset_close = z_offset = -(float)((face_height/2.0) / tan(my_angle) + 
                                         get_base_z_offset());
    
    Point tl, tr, bl, br;

    float half_face_height = face_height / 2.0;
    float half_face_width = face_width / 2.0;

    tl.set (-half_face_width,  half_face_height, .1);
    tr.set ( half_face_width,  half_face_height, .1);
    bl.set (-half_face_width, -half_face_height, .1);
    br.set ( half_face_width, -half_face_height, .1);

    face_A->set_corners(tl, tr, bl, br);

    tl.set ( half_face_width,  half_face_height, -.1);
    tr.set (-half_face_width,  half_face_height, -.1);
    bl.set ( half_face_width, -half_face_height, -.1);
    br.set (-half_face_width, -half_face_height, -.1);

    face_B->set_corners(tl, tr, bl, br);

    if (!showing_face)
        showing_face = face_A;

    //msgout (DEBUG, "set tid = %d\n", face_set->get_texture_id(current_face));

    showing_face->set_texture_id(face_set->get_texture_id(current_face));
}

// there is no entry exit for flips
void FlipArrangement::entry_movement_start(void)
{
    //msgout (DEBUG, "----------4 set tid = %d\n", face_set->get_texture_id(current_face));
    showing_face->set_texture_id(face_set->get_texture_id(current_face));
}

void FlipArrangement::exit_movement_start(void)
{}
void FlipArrangement::exit_movement_cancel(void)
{}
void FlipArrangement::entry_exit_move(void)
{}

int FlipArrangement::is_entry_movement_done(void)
{ return 1; }

int FlipArrangement::is_exit_movement_done(void)
{ return 1; }


void FlipArrangement::set_current_face (int c, int r) 
{
    current_face = c + (r * n_columns);
    current_col = c;
    current_row = r;

    //msgout (DEBUG, "---------- set tid = %d\n", face_set->get_texture_id(current_face));
    showing_face->set_texture_id(face_set->get_texture_id(current_face));

    goto_face_val = goto_face_mv.destination(); 
    goto_face_mv.disable();
}

float FlipArrangement::get_distant_z_offset(void)
{
    return -(z_offset_distance);
}


float FlipArrangement::get_base_z_offset(void)
{ 
    return 0.1;
}


void FlipArrangement::position_faces(void) 
{
    //glTranslatef(0.0f,0.0f, z_offset);
    camera->position.z = -z_offset;

    camera->Look();

    glRotatef(goto_face_val, 0.0f, 1.0f, 0.0f);
    if (spin_val) {
        glRotatef(spin_val,  1.0f,  1.0f,  1.0f);
    }
}

void FlipArrangement::do_movement(void) 
{
    if (!goto_face_mv.at_destination())
        goto_face_mv.change_a_bit();
    else
        clamp_degrees (&goto_face_val);

    if (!spin_mv.at_destination())
        spin_mv.change_a_bit();
    else
        clamp_degrees (&spin_val);
}


float FlipArrangement::goto_face (desktop_coords_t dest) 
{
    if ((dest.column == current_col && dest.row == current_row)
        || dest.column < 0 
        || dest.column >= n_columns
        || dest.row < 0
        || dest.row >= n_rows)
    {
        return 0;
    }

    int newface = dest.column + (dest.row * n_columns);

    int current = current_col + (current_row * n_columns);

    int number = n_columns * n_rows;

    int direction = 1;

    // figure out which way to turn to get to the face.
    // This code was taken from carousel goto_face and can
    // be improved/simplified no doubt.  This still does not
    // work if we are mid-goto.
    if (newface > current) {
        if ( (newface - current) <
             ((current + number) - newface) )
        { 
            // left
            direction = -1;
        } else {
            // right
            direction = 1;
        }
    } else {
        // newface < current

        if ( (current - newface) >
             ((newface + number) - current) )
        {
            // left
            direction = -1;
        } else {
            // right
            direction = 1;
        }
    }

    if (goto_face_mv.at_destination())
        goto_face_mv.init (&goto_face_val, goto_face_val, 
                           goto_face_val + per_face_change * direction,
                           cfg->options->face_change_steps, move_type);
    else
        goto_face_mv.init (&goto_face_val, goto_face_val, 
                           goto_face_mv.destination() + per_face_change,
                           cfg->options->face_change_steps, move_type);

    current_face = newface; 
    current_col = dest.column;
    current_row = dest.row;

    // swap faces
    if (showing_face == face_A) {
        showing_face = face_B;
    } else {
        showing_face = face_A;
    }

    //msgout (DEBUG, "---------1 set tid = %d\n", face_set->get_texture_id(current_face));
    showing_face->set_texture_id(face_set->get_texture_id(current_face));

    return cfg->options->face_change_steps;
}


void FlipArrangement::goto_up(void)
{
    if (spin_mv.at_destination())
        spin_mv.init(&spin_val, 0.0, 360.0, 2*cfg->options->face_change_steps, move_type);
    else
        spin_mv.init(&spin_val, spin_val, spin_mv.destination() + 360.0, 
                     2*cfg->options->face_change_steps, move_type);
}

void FlipArrangement::goto_down(void)
{ }


// this has to by overridden because of the 'current_face' variable
// which is private to this class
void FlipArrangement::goto_random_face(void)
{
    int which_one;

    do {
        which_one = get_randomi (0, (n_columns * n_rows) - 1);
    } while (which_one == (current_col + (current_row * n_rows)));
    
    current_face = which_one;

    Arrangement::goto_face (which_one);
}

void FlipArrangement::goto_right(void) 
{
    current_face++;
    if (current_face == n_faces)
        current_face = 0;   // cycle around

    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() - per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }

    // swap faces
    if (showing_face == face_A) {
        showing_face = face_B;
    } else {
        showing_face = face_A;
    }

    //msgout (DEBUG, "----------2 set tid = %d\n", face_set->get_texture_id(current_face));
    showing_face->set_texture_id(face_set->get_texture_id(current_face));
}

void FlipArrangement::goto_left(void) 
{
    if (current_face == 0)
        current_face = n_faces - 1;  // cycle around
    else
        current_face--;
        
    // we are "unrolling" rows and columns so map the linear
    // "current_face" back to rows and columns
    current_col = current_face % n_columns;
    current_row = current_face / n_columns;

    if (goto_face_mv.at_destination()) {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_val + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    } else {
        goto_face_mv.init (&goto_face_val, 
                           goto_face_val, goto_face_mv.destination() + per_face_change, 
                           cfg->options->face_change_steps, move_type);
    }

    // swap faces
    if (showing_face == face_A) {
        showing_face = face_B;
    } else {
        showing_face = face_A;
    }

    //msgout (DEBUG, "----------3 set tid = %d\n", face_set->get_texture_id(current_face));
    showing_face->set_texture_id(face_set->get_texture_id(current_face));
}


int FlipArrangement::allow_blending(void)
{
    return 0;  // flip looks bad with blending
}

#endif  // if !defined ONE_ARRANGEMENT_ONLY
