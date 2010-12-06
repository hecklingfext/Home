//  ----------------------------------------------------------
//
//  Copyright (C) 2004 Brad Wasson <bard@wassonartistry.com>
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

#include <math.h>
#include <GL/glu.h>
#include "camera.hpp"

Camera::Camera()
{
    position   = Vector(0.0, 0.0, 0.0);
    viewvector = Vector(0.0, 0.0, 0.0);
    upvector   = Vector(0.0, 0.0, 1.0);
}


void Camera::PositionCamera(float position_x, float position_y, float position_z,
                            float viewvector_x, float viewvector_y, float viewvector_z,
                            float upvector_x, float upvector_y, float upvector_z)
{
    position   = Vector(position_x, position_y, position_z);
    viewvector = Vector(viewvector_x, viewvector_y, viewvector_z);;
    upvector   = Vector(upvector_x, upvector_y, upvector_z);
}


void Camera::RotateView(float angle, float x, float y, float z)
{
    Vector new_view;

    Vector view = viewvector - position;        

    float cos_theta = (float)cos(angle);
    float sin_theta = (float)sin(angle);

    new_view.x  = (cos_theta + (1 - cos_theta) * x * x)     * view.x;
    new_view.x += ((1 - cos_theta) * x * y - z * sin_theta) * view.y;
    new_view.x += ((1 - cos_theta) * x * z + y * sin_theta) * view.z;

    new_view.y  = ((1 - cos_theta) * x * y + z * sin_theta) * view.x;
    new_view.y += (cos_theta + (1 - cos_theta) * y * y)     * view.y;
    new_view.y += ((1 - cos_theta) * y * z - x * sin_theta) * view.z;

    new_view.z  = ((1 - cos_theta) * x * z - y * sin_theta) * view.x;
    new_view.z += ((1 - cos_theta) * y * z + x * sin_theta) * view.y;
    new_view.z += (cos_theta + (1 - cos_theta) * z * z)     * view.z;

    viewvector = position + new_view;
}


void Camera::RotateAroundPoint(Vector center, float angle, float x, float y, float z)
{
    Vector new_position;            

    Vector pos = position - center;

    float cos_theta = (float)cos(angle);
    float sin_theta = (float)sin(angle);

    new_position.x  = (cos_theta + (1.0f - cos_theta) * x * x)     * pos.x;
    new_position.x += ((1.0f - cos_theta) * x * y - z * sin_theta) * pos.y;
    new_position.x += ((1.0f - cos_theta) * x * z + y * sin_theta) * pos.z;

    new_position.y  = ((1.0f - cos_theta) * x * y + z * sin_theta) * pos.x;
    new_position.y += (cos_theta + (1.0f - cos_theta) * y * y)     * pos.y;
    new_position.y += ((1.0f - cos_theta) * y * z - x * sin_theta) * pos.z;

    new_position.z  = ((1.0f - cos_theta) * x * z - y * sin_theta) * pos.x;
    new_position.z += ((1.0f - cos_theta) * y * z + x * sin_theta) * pos.y;
    new_position.z += (cos_theta + (1.0f - cos_theta) * z * z)     * pos.z;

    position = center + new_position;
}


void Camera::Look(void)
{
    gluLookAt(position.x, position.y, position.z,  
              viewvector.x, viewvector.y, viewvector.z,  
              upvector.x, upvector.y,  upvector.z);
}
