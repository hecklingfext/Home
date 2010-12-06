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

#ifndef _FACE_HPP
#define _FACE_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include "util.h"
#include "config.hpp"

extern Config *cfg;


class Point {
public:
    vertex_t v;
    Point (float _x = 0, float _y = 0, float _z = 0) {
        v[0] = _x; v[1] = _y; v[2] = _z; v[3] = 1;
    }
    void set (float _x, float _y, float _z) {
        v[0] = _x; v[1] = _y; v[2] = _z; v[3] = 1;
    }
};


class Face {
public:
    Point topleft, topright, bottomleft, bottomright;
    
    Point normal;

    GLuint texture_id;
    float transparency;

    short active;

    uint clist;

    int wireframe;

    Face (int wf = -1) {
        memset (&topleft, 0, sizeof(Point));
        memset (&topright, 0, sizeof(Point));
        memset (&bottomleft, 0, sizeof(Point));
        memset (&bottomright, 0, sizeof(Point));
        memset (&normal, 0, sizeof(Point));
        texture_id = (GLuint)-1;
        transparency = 1.0;
        active = 0;
        clist = (uint)-1;
        wireframe = wf;
    }

    
    void calculate_normal () {

        // tl ---- tr
        //         |
        //         br

        // calculate normal
        float x1 = topleft.v[0] - topright.v[0];
        float y1 = topleft.v[1] - topright.v[1];
        float z1 = topleft.v[2] - topright.v[2];

        float x2 = bottomright.v[0] - topright.v[0];
        float y2 = bottomright.v[1] - topright.v[1];
        float z2 = bottomright.v[2] - topright.v[2];

        normal.v[0] = (y1 * z2) - (y2 * z1);
	normal.v[1] = (z1 * x2) - (z2 * x1);
	normal.v[2] = (x1 * y2) - (x2 * y1);

        // normalize normal ;) 
        float len = (float)sqrt(normal.v[0] * normal.v[0] + 
                                normal.v[1] * normal.v[1] + 
                                normal.v[2] * normal.v[2]);
        normal.v[0] /= len;
        normal.v[1] /= len;
        normal.v[2] /= len;

    }

    void set_list (uint i) {
        clist = i;
    }

    void set_corners(Point tl, 
                     Point tr, 
                     Point bl, 
                     Point br)
    {
        topleft = tl;
        topright = tr;
        bottomleft = bl;
        bottomright = br;
        
        calculate_normal ();
    }

    void set_active () {
        active = 1;
    }

    void set_transparency (float t) {
        transparency = t;
    }

    void set_texture_id (GLuint tid) {
        texture_id = tid;
    }

    GLuint get_texture_id() {
        return texture_id;
    }

    GLuint *get_texture_id_ptr() {
        return &texture_id;
    }

    void render (void) {

        if (texture_id != (GLuint)-1) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
            glEnable(GL_COLOR_MATERIAL);

            //glColor4f(transparency, transparency, transparency, transparency);
            glColor4f(1.0, 1.0, 1.0, transparency);

            //msgout(DEBUG, "======== t=%f\n", transparency);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        } else {
            // no digit or screenshot so show gray color
            glDisable(GL_TEXTURE_2D);

            float v = 0.3;

            switch (cfg->options->frame_color) {
            case COLOR_RED:
                glColor4f(v, 0, 0, transparency); 
                break;
            case COLOR_GREEN:
                glColor4f(0, v, 0, transparency);
                break;
            case COLOR_BLUE:
                glColor4f(0, 0, v, transparency);
                break;
            case COLOR_LIGHTBLUE:
                glColor4f(0, v, v, transparency);
                break;
            case COLOR_GRAY:
                glColor4f(v, v, v, transparency);
                break;
            case COLOR_WHITE:
                glColor4f(1.0, 1.0, 1.0, transparency);
                break;
            case COLOR_PURPLE:
                glColor4f(v, 0, v, transparency);
                break;
            case COLOR_YELLOW:
                glColor4f(v, v, 0, transparency);
                break;
            default: // gray
                glColor4f(v, v, v, transparency);
                break;
            }

            if (wireframe == 1 ||
                (wireframe == -1 && cfg->options->use_wireframe))
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            glLineWidth (2);
            glDisable (GL_LIGHTING);
        }


        if (clist != (uint)-1) {
            glCallList (clist);
        } else {
            glBegin(GL_QUADS);

            glNormal3fv( normal.v );

            glTexCoord2f (0.0f, 0.0f);
            glVertex3fv(topleft.v);

            glTexCoord2f (1.0f, 0.0f);
            glVertex3fv(topright.v);

            glTexCoord2f (1.0f, 1.0f);
            glVertex3fv(bottomright.v);

            glTexCoord2f (0.0f, 1.0f);
            glVertex3fv(bottomleft.v);

            glEnd();

        }

        glEnable (GL_LIGHTING);

        // restore to fill
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void load_texture_data (int texture_size, unsigned char *data) {

        if (texture_id == (GLuint)-1)
            glGenTextures(1, &texture_id);

        glBindTexture(GL_TEXTURE_2D, texture_id);   
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, MIN_FILTER);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, MAX_FILTER);
        
#ifndef GL_COMPRESSED_RGBA
#define GL_COMPRESSED_RGBA GL_RGBA
#endif
        if (cfg->options->glcompression)
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_COMPRESSED_RGBA, texture_size, texture_size, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture_size, texture_size, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

};

#endif // _FACE_HPP
