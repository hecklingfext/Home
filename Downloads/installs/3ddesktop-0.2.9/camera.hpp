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

#ifndef _CAMERA_HPP
#define _CAMERA_HPP

using namespace std;

class Vector
{
public:
    
    Vector() { x = y = z = 0.0f;}

    Vector(float _x, float _y, float _z) 
    { 
        x = _x; y = _y; z = _z;
    }

    Vector operator+(Vector vector)
    {
        return Vector(vector.x + x, vector.y + y, vector.z + z);
    }

    Vector operator-(Vector vector)
    {
        return Vector(x - vector.x, y - vector.y, z - vector.z);
    }
    
    Vector operator*(float num)
    {
        return Vector(x * num, y * num, z * num);
    }

    Vector operator/(float num)
    {
        return Vector(x / num, y / num, z / num);
    }

    float x, y, z;
};


class Camera {

public:

    Vector position;
    Vector viewvector;
    Vector upvector;

    Camera();
    
    void PositionCamera(float position_x, float position_y, float position_z,
                        float viewvector_x, float viewvector_y, float viewvector_z,
                        float upvector_x, float upvector_y, float upvector_z);
    void RotateView(float angle, float x, float y, float z);
    void RotateAroundPoint(Vector center, float angle, float x, float y, float z);
    void Look(void);
};


#endif // _CAMERA_HPP
