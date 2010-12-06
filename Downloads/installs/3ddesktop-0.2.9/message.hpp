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
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// used in message passing with 3ddesktop daemon 
typedef struct threedeedesk_command {
    long mtype;
    char view[50];
    int goto_column;
    int goto_row;
    int zoom;
    FaceType_t face_type;
    int acquire_sleep;
    int reload;
    float face_change_steps;
    float zoom_steps;
    int disable_exit_after_goto;
    int disable_random_fun;
    int reverse_mousewheel;
    int swap_mousebuttons;
    int alt_mousebuttons;
} threedeedesk_command_t;


// This class is supposed to encapsulate the messaging functionality
// between the client (3ddesk) and the daemon (3ddeskd).  If the
// messaging mechanism should ever be changed its easy to change cause
// its all here.  This "manager" is used by both client and daemon
// (one to send and the other to recieve).
class MessageManager {
private:
    int msqid;
public:

    // modified directly by client and daemon
    threedeedesk_command_t  msg;


    enum msgtype_t {
        WAIT = 0,
        NOWAIT,
        CREATE,
        NOCREATE,
    };

    // Can't use msgout or end_program because this is used by both the
    // client and daemon.

    MessageManager() {
        msqid = -1;

        msg.mtype = MTYPE_ACTIVATE;
        msg.view[0] = 0;
        msg.goto_column = NO_GOTO;
        msg.goto_row = NO_GOTO;
        msg.zoom = -1;
        msg.face_type = FACE_TYPE_NONE;
        msg.acquire_sleep = 0;
        msg.reload = 0;
        msg.face_change_steps = 0;
        msg.zoom_steps = 0;
        msg.disable_exit_after_goto = 0;
	msg.disable_random_fun = 0;
        msg.reverse_mousewheel = 0;
        msg.swap_mousebuttons = 0;
        msg.alt_mousebuttons = 0;
    };


    int open (msgtype_t t = NOCREATE) {
        key_t key;
        key = ftok("/bin/sh", 222);
        if (key == (key_t)-1)
            return -1;
        
        int o = 0666;
        if (t == CREATE)
            o |= IPC_CREAT;

        msqid = msgget(key, o);
        if (msqid == -1)
            return -1;

        if (t == CREATE)
            // clear the queue
            while (recieve(NOWAIT) == 0 && errno != ENOMSG)
                ;

        return msqid;
    };

    int close () {
        if (msqid != -1)
            if (msgctl(msqid, IPC_RMID, NULL) == -1) 
                return -1;

        return 0;
    };

    int send () {
        return msgsnd(msqid, (struct msgbuf *)&msg, sizeof(msg) - sizeof(long), 0);

    };

    // returns -1 for error or 0 for success or NOMSG
    int recieve (msgtype_t t = WAIT) {
        int rc;
        do {
            errno = 0;
            rc = msgrcv(msqid, (struct msgbuf *)&msg, sizeof(msg)- sizeof(long), 0, 
                            t == NOWAIT ? IPC_NOWAIT : 0);
        } while (rc < 0 && errno == EINTR);

        if (rc < 0) {
            if (errno == ENOMSG)  // this is ok for IPC_NOWAIT
                return 0;
            
            //msgout (ERROR, "msgrcv failed: %s\n", strerror(errno));
            return -1;
        }

        return 0;
    };

    // remove all messages out of the message queue 
    // returns -1 for error or 0 for success
    int flush () {
        int rc;
        while (1) {  // until all messages are out 
            do {
                errno = 0;
                rc = msgrcv(msqid, (struct msgbuf *)&msg, sizeof(msg)- sizeof(long), 0, 
                            IPC_NOWAIT);
            } while (rc < 0 && errno == EINTR);
            
            if (rc < 0) {
                if (errno == ENOMSG)  // this is ok for IPC_NOWAIT
                    return 0;
            
                //msgout (ERROR, "msgrcv failed: %s\n", strerror(errno));
                return -1;
            }
        }

    };
};
