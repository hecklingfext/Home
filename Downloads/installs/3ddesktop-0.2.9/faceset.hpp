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

#ifndef _FACESET_HPP
#define _FACESET_HPP

#include "3ddesk.h"
#include "util.h"
#include "config.hpp"

using namespace std;
#include <vector>


// container class for a set of faces
class FaceSet {
private:
    int number_of_faces;
    int cols;
    int rows;
    

    vector <Face *> faces;

public:
    FaceSet (int r, int c) {
        cols = c;
        rows = r;
        number_of_faces = rows * cols;

        for (int i = 0; i < number_of_faces; i++) {
            Face *f = new Face();
            if (f == NULL) {
                msgout (ERROR, "Out of memory: faceset constructor\n");
                end_program(-1);
            }

            faces.push_back(f);
        }

        //load_digit_textures();

    };

    void reconfigure (int r, int c) {
        int old_num = number_of_faces;
        int old_cols = cols;

        cols = c;
        rows = r;
        number_of_faces = rows * cols;

        if (old_num > number_of_faces) {
            // deleting faces
            for (int i = 0; i < old_num - number_of_faces; i++) {
                msgout (DEBUG, "uuuu deleted %d\n", i + number_of_faces);
                delete faces[i + number_of_faces];
            }
            faces.erase (faces.begin() + number_of_faces, faces.end());
            reset_face_textures();

        } else if (old_num < number_of_faces) {
            // adding faces
            for (int i = 0; i < number_of_faces - old_num; i++) {
                msgout (DEBUG, "uuuu new face %d\n", i + old_num);
                Face *f = new Face();
                if (f == NULL) {
                    msgout (ERROR, "Out of memory: faceset constructor\n");
                    end_program(-1);
                }
                faces.push_back(f);
                //faces[i + old_num] = f;  this is a no-no!
            }
            if (cols != old_cols && rows > 1)  // if columns changed must start new
                reset_face_textures();

        } else {
            // same cols and rows - do nothing
        }

    };

    void reset_face_textures() {
        for(int i = 0; i < number_of_faces; i++) {
            faces[i]->set_texture_id ((uint)-1);
        }
    };

    // this function returns an array of indexes indicating what order
    // faces should be drawn when they are in a circle.  the current
    // face first then the ones on either side then the next two found
    // on the left and right in the circle and so on etc.  this is
    // done so transparency and blending look right.

    void get_draw_sequence(int len, int start, int *out)
    {
        int i = 1;
        int a, b;
        int level;
        out[0] = start;
        for (level = 1; level <= len/2; level++) {
            a = start - level;
            b = start + level;
            if (a < 0) a += len;
            if (b >= len) b -= len;
            out[i++] = a;
            if (i >= len) break;
            out[i++] = b;
        }
    };

    // this function renders faces that are assumed to be
    // in a circle and renders them back to front
    void render_all (int current_face) {
        int *order = new int[number_of_faces];

        int tmp = current_face - number_of_faces/2;
        if (tmp < 0) tmp += number_of_faces;
        get_draw_sequence(number_of_faces, tmp, order);

        for (int i = 0; i < number_of_faces; i++) {
            //msgout(DEBUG, "uuuuu rendering %d (%d)\n", order[i], i);
            faces[ order[i] ]->render();
        }

        delete order;
    };

    void render_all () {
        int i;
        for (i = 0; i < number_of_faces; i++)
            faces[i]->render();
    };

    uint get_texture_id (int i) {
        if (i >= number_of_faces) {
            msgout (ERROR, "BUG: FaceSet::get_texture_id i >= n\n");
            return (uint)-1;
        }        
        return faces[i]->get_texture_id();
    };

    void load_texture_data (int i, int size, unsigned char *data) {
        if (i >= number_of_faces) {
            msgout (ERROR, "BUG: FaceSet::load_texture_data i >= n\n");
            return;
        }        
        faces[i]->load_texture_data(size, data);
    };

    void set_list (int i, uint l) {
        if (i >= number_of_faces) {
            msgout (ERROR, "BUG: FaceSet::set_list i >= n\n");
            return;
        }
        faces[i]->set_list (l);
    };        

    void set_active (int i) {
        if (i >= number_of_faces) {
            msgout (ERROR, "BUG: FaceSet::set_active i >= n\n");
            return;
        }
        faces[i]->set_active ();
    };

    void set_face (int i, Point tl, Point tr, Point bl, Point br) {
        if (i >= number_of_faces) {
            msgout (ERROR, "BUG: FaceSet::set_face i >= n\n");
            return;
        }

        faces[i]->set_corners(tl, tr, bl, br);
    };

    void set_transparency (float t) {
        for (int i = 0; i < number_of_faces; i++) 
            faces[i]->set_transparency(t);
    };

};

#endif // _FACESET_HPP
