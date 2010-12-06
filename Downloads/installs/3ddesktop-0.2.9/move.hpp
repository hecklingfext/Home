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
#ifndef _MOVE_HPP
#define _MOVE_HPP

#include <math.h> // for sin etc
#include "3ddesk.h"



class Movement {
protected:
    float start_value;
    float end_value;
    float *current;
    float step;
    float start_iter;
    float end_iter;
    float iter;
    bool inited;
    float scale_factor;
    
    float (Movement::*pos_func)(float);

    // These are all a bunch of different movements to play with
    // Basically this means using different functions to determine
    // position which makes the "thing" being moved apear to move
    // at different rates.  For example linear is just a constant
    // speed (y = t where y is position and t is time - take the
    // derivative of this and you get velocity as a constant)

    float sin_func       (float t) { return (sin(t) + t); }
    float x_squared_func (float t) { return (4 - t*t); }
    float log_func       (float t) { return (log (t)); }
    float e_func         (float t) { return (5 - exp(-t)); }
    float linear_func    (float t) { return (t); }
    float circle_func    (float t) { return ( sqrt(9 - t*t) ); }
    float waver_func     (float t) { return ( (-sin(t)/(2*t)) + 1 ); }
    float plain_sine_func(float t) { return (sin(t)); }
    

public:
    enum MOVE_TYPE {
        MOVE_TYPE_WACK,
        MOVE_TYPE_SMOOTH,
        MOVE_TYPE_WACK2,
        MOVE_TYPE_WACK3,
        MOVE_TYPE_WACK4,
        MOVE_TYPE_WACK5,
        MOVE_TYPE_OVERSHOOT,
        MOVE_TYPE_CONSTANT,
        MOVE_TYPE_WAVER,
        MOVE_TYPE_SINE,
        MOVE_TYPE_HALF_SINE
    };
    MOVE_TYPE type;


    Movement () { inited = false; current = NULL;}
    
    void init (float *ref, float sv, float ev, float times, MOVE_TYPE _type) {

        type = _type;
        current = ref;
        start_value = *current = sv;
        end_value = ev;

        float min = 999, max = 999;

        switch (type) {
        case MOVE_TYPE_WACK5:
            pos_func = &Movement::sin_func;
            start_iter = 2.0 * PI;
            end_iter = 3.0 * PI;
            break;
        case MOVE_TYPE_SMOOTH:
            pos_func = &Movement::sin_func;
            start_iter = PI;
            end_iter = 3.0 * PI;
            break;
        case MOVE_TYPE_WACK2:
            pos_func = &Movement::x_squared_func;
            start_iter = -2;
            end_iter = 0;
            break;
        case MOVE_TYPE_WACK3:
            pos_func = &Movement::log_func;
            start_iter = 1;
            end_iter = 30;
            break;
        case MOVE_TYPE_WACK4:
            pos_func = &Movement::e_func;
            start_iter = 0;
            end_iter = 5;
            break;
        case MOVE_TYPE_CONSTANT:
            pos_func = &Movement::linear_func;
            start_iter = 0;
            end_iter = 5;
            break;
        case MOVE_TYPE_WACK:
            pos_func = &Movement::circle_func;
            start_iter = -2.8;
            end_iter = .1;
            break;
        case MOVE_TYPE_OVERSHOOT:
            pos_func = &Movement::circle_func;
            start_iter = -2.8;
            end_iter = 1.1;
            break;
        case MOVE_TYPE_WAVER:
            pos_func = &Movement::waver_func;
            start_iter = 1;
            end_iter = 39; //26.5;
            break;
        case MOVE_TYPE_SINE:
            pos_func = &Movement::plain_sine_func;
            start_iter = 0;
            end_iter = 2 * PI;
            max = 1;
            min = -1;
            break;
        case MOVE_TYPE_HALF_SINE:
            pos_func = &Movement::plain_sine_func;
            start_iter = 0;
            end_iter = PI;
            max = 1;
            min = 0;
            break;
        default:
            //panic();
            break;
        }
        
        if (max == 999) {  // if not set use function
            max = (this->*pos_func)(end_iter);
            min = (this->*pos_func)(start_iter);
        }


        iter = start_iter;

        // this is the trick to it all -- you scale the position
        // function to the actual distance you are travelling.  then
        // it doesn't matter the range or scale of the position
        // function.
        scale_factor = (ev - sv) / (max - min);

        step = (end_iter - start_iter) / times;

        inited = true;
    }

    bool at_destination (void) {
        if ((iter > end_iter) || !inited) {

            // this enforces that we end up where we want to...
            if (current)
                *current = end_value;

            return true;
        }

#if 0
        if (direction > 0) {
            if (*current >= end_value) {
                printf("value end:: %lf\n", *current);
                *current = end_value;
                return true;
            }
        } else {
            if (*current <= end_value) {
                printf("value end: %lf\n", *current);
                *current = end_value;
                return true; 
            }
        }
#endif
        return false;
    }
    float value       (void) { return (current ? *current : 0.0); }
    float destination (void) { return end_value; }
    void  disable     (void) { iter = 1; end_iter = 0; current = NULL; }

    void  change_a_bit  (void) {
        if (at_destination()) 
            return;
        *current = (scale_factor * ((this->*pos_func)(iter) - (this->*pos_func)(start_iter)))
                    + start_value;

        iter += step;
    }

};

#endif // _MOVE_HPP
