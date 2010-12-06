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

#ifndef _EVENT_HPP
#define _EVENT_HPP

enum event_t {
    PROGRAM_START,
    ENTRY_MOVEMENT_START,
    ENTRY_MOVEMENT_STOP,
    EXIT_MOVEMENT_START,
    EXIT_MOVEMENT_STOP,
    GOTO_FACE_COMPLETE
};


class Event {
private:
public:
    // this is the event type that will trigger this event
    event_t type;

    void (*function) (Event *);

    void *data;

    int reoccurring;

    Event (event_t _type, 
           void (*_function)(Event *), 
           void *_data = NULL, 
           int _reoccurring = 0 ) 
    {
        type = _type;
        function = _function;
        data = _data;
        reoccurring = _reoccurring;
    };
    
    void activate () {
        function(this);
    };

};


class EventManager {
private:
    
    list <Event *> events;

public:
    EventManager() {};

    void print_events_in_queue() {
        list<Event *>::iterator k;
        for (k = events.begin(); k != events.end(); ++k) {
            Event *e = *k;
            printf(":: Event %d  0x%x\n", e->type, (unsigned int)(e->function));
        }
    }

    void trigger_event(event_t event) {
        // step through events and activate any that are this event.
        list<Event *>::iterator k;
        bool dont_increment;
        k = events.begin(); 
        while (k != events.end()) {
            dont_increment = false;
            //printf ("^v^v^ trying %d  0x%x\n", (*k)->type, (*k)->function);
            if ((*k)->type == event) {
                
                Event *e = *k;
                if (!e->reoccurring) {
                    events.erase(k++);
                    dont_increment = true;
                }
                    
                e->activate();
                
                if (!e->reoccurring)
                    delete e;
            }
            if (!dont_increment)
                ++k;
        }
    };

    // step through events and remove any that are this event.
    void remove_event(event_t type, void (*function)(Event *))
    {
        list<Event *>::iterator k;
        k = events.begin(); 
        while (k != events.end()) {
            if (((*k)->type == type) && ((*k)->function == function)) {
                events.erase(k++);
            } else {
                ++k;
            }
        }
    };
    
    void add_event (Event *e) {
        events.push_back(e);
    };

    void add_event (event_t type,
                    void (*function)(Event *),
                    void *data = NULL,
                    int reoccurring = 0) 
    {
        // deleted in trigger_event after activation
        Event *e = new Event (type, function, data, reoccurring);
        if (!e) {
            msgout (ERROR, "out of memory: Event\n");
            end_program(-1);
        }
        events.push_back(e);
    }
                    

};


#endif // _EVENT_HPP
