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

#ifndef _ARRANGE_HPP
#define _ARRANGE_HPP

#include "3ddesk.h"
#include "face.hpp"
#include "faceset.hpp"
#include "move.hpp" // for Movement
#include "xutil.hpp"
#include "config.hpp"
#include "camera.hpp"


// ===================================================================
//  Arranagement - abstract base class 
//    This defines the interface to the derived classes below.  Allows
//    a generic pointer to "Arranagement" to be used whose calls to the
//    virtual functions are then sent to the derived class.
// ===================================================================
class Arrangement {
protected:
    //int number_of_faces;
    int current_face;

    int n_columns;
    int n_rows;
    
    int current_col;
    int current_row;

    Movement::MOVE_TYPE move_type;

    float face_width;
    float face_height; 

    Movement oscillation;
    float oscillation_value;

    // entry/exist z offset speed
    float ee_speed_z;

    Movement z_off_mv;

    // the current z offset changed by the above movement
    float z_offset;

    // this is the z offset that makes the current face match the screen
    // (all the edges of the quad are aligned with the edges of the
    // screen) It's calculated dynamically based on the screen dimensions
    float z_offset_close;

    // this is the space between the viewer and the face at a distance
    float z_offset_distance;

    Config *cfg;

    FaceSet *face_set;

    Camera *camera;

    GLuint face_list;

    Movement goto_face_mv;
    float goto_face_val;
    float per_face_change;

public:
    Arrangement (FaceSet *fs, Config *c);
    virtual ~Arrangement ();

    //virtual ~Arrangement() { };
    
    virtual int cant_exit_in_goto(void);

    virtual void goto_right(void);
    virtual void goto_left(void);

    virtual void goto_up(void);
    virtual void goto_down(void);
    virtual void goto_random_face(void);

    virtual float goto_face (int face);
    virtual float goto_face (int col, int row);
    virtual float goto_face(desktop_coords_t dest);
    virtual int in_goto (void);

    // methods that must be implemented by derived classes
    virtual int get_type(void) = 0;
    virtual float get_base_z_offset(void) = 0;
    virtual float get_distant_z_offset(void) = 0;
    virtual void do_movement(void) = 0;
    virtual void position_faces(void) = 0;
    virtual void position_lighting(void);
    virtual void render();
    virtual void make_display_list(void);
    virtual void set_current_face (int col, int row);
    virtual void calculate_faces (VDesktops & vdesks) = 0;

    // movement hooks for controling entry and exit movements (zoom
    // out and zoom in etc)
    virtual void entry_movement_start(void);
    virtual void entry_exit_move(void);
    virtual void exit_movement_start(void);
    virtual void exit_movement_cancel(void);
    virtual int is_entry_movement_done(void);
    virtual int is_exit_movement_done(void);

    virtual int allow_blending(void);

    // "locally" defined functions
    int get_current_column(void);
    int get_number_of_columns(void);
    int get_current_row(void);
    int get_number_of_rows(void);
    float get_current_z_offset(void);
    void set_z_offset_distance(float zod);
    void set_transparency (float t);
    void set_active (int i);
};




class CarouselArrangement : public Arrangement {
private:
    Movement x_rot_mv;

    float side_angle;

    float x_rot_final;
    float X_Rot;

    float radius;

    //int current_face;
    int n_faces;

public:
    CarouselArrangement(VDesktops &vdesks, float fw, float fh, 
			FaceSet *fs, Config *c, Camera *cam);

    void calculate_faces(VDesktops & vdesks);

    void goto_random_face(void);
    void goto_right(void);
    void goto_left(void);

    int get_type(void);
    void set_current_face (int c, int r);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 
    void do_movement(void);
    void position_faces(void);

    // Entry/Exit movement functions
    void entry_movement_start(void);
    void exit_movement_start(void);
    void exit_movement_cancel(void);
    void entry_exit_move(void);
};


#if !defined (ONE_ARRANAGEMENT_ONLY)

class CylinderArrangement : public Arrangement {
private:
    float two_face_gap;

    float side_angle;

    Movement x_rot_mv;

    float X_Rot;

    float x_rot_final;

    //int current_face;
    int n_faces;

public:
    CylinderArrangement(VDesktops &vdesks, float fw, float fh, 
			FaceSet *fs, Config *c, Camera *cam);

    void calculate_faces(VDesktops & vdesks);

    void goto_random_face(void);
    void goto_right(void);
    void goto_left(void);

    int get_type(void);
    void set_current_face (int c, int r);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 
    void do_movement(void);
    void position_faces(void);

    // Entry/Exit movement functions
    void entry_movement_start(void);
    void exit_movement_start(void);
    void exit_movement_cancel(void);
    void entry_exit_move(void);

};


class ViewMasterArrangement : public Arrangement {
private:
    //int current_face;
    int n_faces;

    float between_angle;
    float center_distance;

public:
    ViewMasterArrangement(VDesktops &vdesks, float fw, float fh,
                          FaceSet *fs, Config *c, Camera *cam);

    void calculate_faces(VDesktops & vdesks);

    int get_type(void);
    void goto_random_face(void);
    void goto_right(void);
    void goto_left(void);

    void set_current_face (int c, int r);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 
    void do_movement(void);
    void position_faces(void);
    void render();
};



class PriceIsRightArrangement : public Arrangement {
private:
    //int current_face;
    int n_faces;

    float side_angle;

    float radius;
public:
    PriceIsRightArrangement(VDesktops &vdesks, float fw, float fh,
                            FaceSet *fs, Config *c, Camera *cam);

    void calculate_faces(VDesktops & vdesks);

    int get_type(void);
    void goto_random_face(void);
    void goto_right(void);
    void goto_left(void);

    void set_current_face (int c, int r);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 
    void do_movement(void);
    void position_faces(void);
};


class LinearArrangement : public Arrangement {
private:
    float between;

    Movement x_trans_mv;
    float X_Trans;

    Movement y_trans_mv;
    float Y_Trans;

    Movement spin_mv;
    float spin_z_rot;

public:
    LinearArrangement(VDesktops &vdesks, float fw, float fh,
                      FaceSet *fs, Config *c, Camera *cam);

    void calculate_faces(VDesktops & vdesks);
    int get_type(void);
    void set_current_face (int col, int row);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 

    void goto_right(void);
    void goto_left(void);
    void goto_up(void);
    void goto_down(void);

    int in_goto (void);

    float goto_face(desktop_coords_t dest);
    void do_movement(void);
    void position_faces(void);
    void position_lighting(void);
    void render();

    // Entry/Exit movement functions which add spinning on exit
    void entry_exit_move(void);
    void exit_movement_start(void);
    void exit_movement_cancel(void);

};


class FlipArrangement : public Arrangement {
private:
    Face *face_A;
    Face *face_B;

    Face *showing_face;

    Movement spin_mv;
    float spin_val;

    //GLint texture[MAX_FACES];

    //int current_face;
    int n_faces;
public:
    FlipArrangement(VDesktops &vdesks, float fw, float fh,
                    FaceSet *fs, Config *c, Camera *cam);

    ~FlipArrangement ();

    int cant_exit_in_goto(void);
    int in_goto (void);

    void calculate_faces(VDesktops & vdesks);
    int get_type(void);
    void render();
    void make_display_list(void);
    void load_texture_data (int i, int texture_size, unsigned char *data);
    void set_current_face (int c, int r);
    float get_base_z_offset(void); 
    float get_distant_z_offset(void); 
    void do_movement(void);
    void position_faces(void);

    void goto_down(void);
    void goto_up(void);
    float goto_face (desktop_coords_t dest);
    void goto_random_face(void);
    void goto_right(void);
    void goto_left(void);

    int allow_blending(void);

    // Entry/Exit movement functions
    void entry_movement_start(void);
    void exit_movement_start(void);
    void exit_movement_cancel(void);
    void entry_exit_move(void);
    int is_entry_movement_done(void);
    int is_exit_movement_done(void);
};




#endif // if !defined (ONE_ARRANAGEMENT_ONLY)

#endif // _ARRANAGE_HPP
