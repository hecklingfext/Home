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
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <getopt.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/time.h> // struct timeval

#include "3ddesk.h"
#include "message.hpp"

#ifdef printf
#undef printf
#endif

static int start_daemon(void);
static int server_running(void);
static int wait_for_server_to_start(void);

static void 
usage_and_bye (void)
{
    printf ("usage: 3ddesk [ OPTIONS ]\n\n"
            "Activates the 3d Desktop.  3ddeskd daemon must be running.\n"
            "Where OPTIONS are:\n"
            "     --version         Display version information\n"
            "     --view=xxx        Uses the options from the view in 3ddesktop.conf\n"
            "     --mode=xxx        Sets the arrangement mode \n"
            "                        (one of carousel, cylinder, linear, viewmaster,\n"
            "                         priceisright, flip, or random)\n"
            "     --acquire[=#]     Grab images for all the desktops by cycling thru\n"
            "                        (sleep for x millisecs at each screen for refresh)\n"
            "     --acquirecurrent  Grab image for current desktop\n"
            "     --nozoom          Disable the zoom out\n"
            "     --gotoright       Goto the desktop to the right\n"
            "     --gotoleft        Goto the desktop to the left\n"
            "     --gotoup          Goto the desktop to the up\n"
            "     --gotodown        Goto the desktop to the down\n"
            "     --goto=#          Goto specified column (deprecated)\n"
            "     --gotocolumn=#    Goto specified column\n"
            "     --gotorow=#       Goto specified row\n"
            "     --dontexit        Don't exit after a goto\n"
            "     --stop            Stop 3ddesktop (kill 3ddeskd daemon)\n"
            "     --reload          Force a reload of 3ddesktop.conf\n"
	    "     --noautofun       Don't Automatically turn on Fun Mode\n"
            "     --revmousewheel   Reverse the mousewheel\n"
            "     --swapmousebuttons Swap the mousebuttons\n"
            "     --altmousebuttons Use alternate mousebuttons scheme\n"
            "     --justswitch      Just switch desktops and acquire without graphics\n\n"
        );
    exit (1);
}

int verbose = 0;

int main(int argc, char **argv)
{

    MessageManager msgmgr;
    
    static struct option long_options[] = {
        {"nozoom", 0, 0, 0},
        {"goto", 1, 0, 0},
        {"gotoright", 0, 0, 0},
        {"gotoleft", 0, 0, 0},
        {"gotoup", 0, 0, 0},
        {"gotodown", 0, 0, 0},
        {"mode", 1, 0, 0},
        {"acquire", 2, 0, 0},
        {"acquirecurrent", 0, 0, 0},
        {"stop", 0, 0, 0},
        {"gotocolumn", 1, 0, 0},
        {"gotorow", 1, 0, 0},
        {"view", 1, 0, 0},
        {"zoom", 0, 0, 0},
        {"reload", 0, 0, 0},
        {"changespeed", 1, 0, 0},
        {"zoomspeed", 1, 0, 0},
        {"dontexit", 0, 0, 0},
	{"noautofun", 0, 0, 0},
	{"revmousewheel", 0, 0, 0},
	{"swapmousebuttons", 0, 0, 0},
	{"altmousebuttons", 0, 0, 0},
	{"justswitch", 0, 0, 0},
        {"version", 0, 0, 0},
        {0, 0, 0, 0}
    };

    float tmp;
    int c;
    int option_index;
    while (1) {

        c = getopt_long (argc, argv, "v",
                         long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0: // long option

            switch (option_index) {
            case 0: // 'nozoom'
                msgmgr.msg.zoom = 0;
                break;
            case 1: // 'goto'  ---- use is deprecated (in favor of gotocolumn)
                msgmgr.msg.goto_column = atoi (optarg);
                if (msgmgr.msg.goto_column < 1) 
                    usage_and_bye();
                break;
            case 2: // 'gotoright'
                msgmgr.msg.goto_column = GOTO_FACE_RIGHT;
                break;
            case 3: // 'gotoleft'
                msgmgr.msg.goto_column = GOTO_FACE_LEFT;
                break;
            case 4: // 'gotonext'
                msgmgr.msg.goto_row = GOTO_FACE_UP;
                break;
            case 5: // 'gotoprev'
                msgmgr.msg.goto_row = GOTO_FACE_DOWN;
                break;
            case 6: // 'mode'
                if (strcmp(optarg, "cylinder") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_CYLINDER;
                } else if (strcmp(optarg, "linear") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_LINEAR;
                } else if (strcmp(optarg, "viewmaster") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_VIEWMASTER;
                } else if (strcmp(optarg, "carousel") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_CAROUSEL;
                } else if (strcmp(optarg, "priceisright") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_PRICEISRIGHT;
                } else if (strcmp(optarg, "flip") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_FLIP;
                } else if (strcmp(optarg, "random") == 0) {
                    msgmgr.msg.face_type = FACE_TYPE_RANDOM;
                } else {
                    usage_and_bye();
                }
                break;
            case 7: // 'acquire'
                msgmgr.msg.mtype = MTYPE_ACQUIRE;
                if (optarg) {
                    int tmpint = atoi (optarg);
                    // must be between 10ms and 3sec for now
                    if (10 < tmpint && tmpint < 3000)
                        msgmgr.msg.acquire_sleep = tmpint;
                } else {
                    msgmgr.msg.acquire_sleep = 700;
                }
                break;
            case 8: // 'acquirecurrent'
                msgmgr.msg.mtype = MTYPE_ACQUIRE_CURRENT;
                break;
            case 9: // 'stop'
                msgmgr.msg.mtype = MTYPE_STOP;
                printf ("Stopping 3ddesktop daemon...\n");
                break;
            case 10: // 'gotocolumn'
                msgmgr.msg.goto_column = atoi (optarg);
                if (msgmgr.msg.goto_column < 1) 
                    usage_and_bye();
                break;
            case 11: // 'gotorow'
                msgmgr.msg.goto_row = atoi (optarg);
                if (msgmgr.msg.goto_row < 1) 
                    usage_and_bye();
                break;
            case 12: // 'view'
                strcpy(msgmgr.msg.view, optarg);
                break;
            case 13: // 'zoom'
                msgmgr.msg.zoom = 1;
                break;
            case 14: // 'reload'
                msgmgr.msg.reload = 1;
                break;
            case 15: // 'changespeed'
                tmp = atof(optarg);
                if (tmp >= 1.0 && tmp <= 300.0) {
                  msgmgr.msg.face_change_steps = tmp;
                }
                break;
            case 16: // 'zoomspeed'
                tmp = atof(optarg);
                if (tmp >= 1.0 && tmp <= 300.0) {
                  msgmgr.msg.zoom_steps = tmp;
                }
                break;
            case 17: // 'dontexit'
                msgmgr.msg.disable_exit_after_goto = 1;
                break;
	    case 18: // 'noautofun'
		msgmgr.msg.disable_random_fun = 1;
                break;
	    case 19: // 'revmousewheel'
		msgmgr.msg.reverse_mousewheel = 1;
                break;
	    case 20: // 'swapmousebuttons'
		msgmgr.msg.swap_mousebuttons = 1;
                break;
	    case 21: // 'altmousebuttons'
		msgmgr.msg.alt_mousebuttons = 1;
                break;
            case 22: // 'justswitch'
                msgmgr.msg.mtype = MTYPE_JUSTSWITCH;
                break;
            case 23: // 'version'
                printf("3ddesktop %s (3ddesk client)\nWritten by Brad Wasson.\n\nCopyright (C) 2002 - 2005 Brad Wasson.\n"
                       "This is free software; see the source for copying conditions.  There is NO\n"
                       "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n"
                       "This binary compiled %s %s.\n\n",
                       VERSION, __DATE__, __TIME__);
                exit(0);
                break;
            }
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            usage_and_bye();
            break;
        }
    }


    if (msgmgr.open() < 0 || !server_running()) {
        if (msgmgr.msg.mtype == MTYPE_STOP) {
            // already stopped...
            fprintf(stderr, "Server not running.\n");
            exit (1);
        }

        fprintf(stderr, "Attempting to start 3ddesktop server.\n");
  
        if (start_daemon() < 0) {
            fprintf(stderr, "Could not start server.\n"
                    "Try starting manually (3ddeskd)\n\n");
            exit(1);
        }

        if (wait_for_server_to_start() == 0) {
            if (msgmgr.open() < 0) {
                fprintf(stderr, "Can't get message queue: %s\n", strerror(errno));
                fprintf(stderr, "Maybe 3ddeskd server not started?\n");
                exit (1);
            }
        } else {
            fprintf(stderr, "Could not find server.\n"
                    "Try starting manually (3ddeskd)\n\n");
            exit (1);
        }
    }

    if (msgmgr.send() < 0) {
        fprintf(stderr, "Failed to send server a message\n");
        exit (1);
    }

    return 0;
}

static int
start_daemon()
{
    int pid;

    // since 3ddeskd puts itself in the background we could just use
    // system here I think but this to me is more direct

    pid = fork();

    if (pid < 0) {  // fork error
        fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        return -1;
    }

    if (!pid) {  // child
        // 3ddeskd should be in path or current dir
        execlp("3ddeskd", "3ddeskd", NULL);
        execlp("./3ddeskd", "3ddeskd", NULL);
        execlp("/usr/bin/3ddeskd", "3ddeskd", NULL);
        execlp("/usr/local/bin/3ddeskd", "3ddeskd", NULL);
        fprintf(stderr, "Failed to execute 3ddeskd: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static int 
server_running(void)
{
    int semid;
    key_t key = ftok("/bin/sh", 99);
    if ((key >= 0)
        && ((semid = semget(key, 1, 0666)) >= 0)
        && (semctl(semid, 0, GETVAL, NULL) > 0))
        return true;

    return false;
}


// returns 0 for server was found to be started
// returns -1 for error or server was not found to have started
static int
wait_for_server_to_start(void)
{
    key_t key = ftok("/bin/sh", 99);
    if (key < 0) {
        fprintf(stderr, "wait_for_server: ftok failed: %d: %s\n", errno, strerror(errno));
        return -1;
    }

    struct timeval tm;

    // poll the demon semaphore for a few seconds at most
#define SECONDS_TO_WAIT 5

    int i;
    for (i = 0; i < SECONDS_TO_WAIT * 100; i++) {
        if (server_running())
            break;
        
        tm.tv_sec = 0;
        tm.tv_usec = 10000;
        select (0, 0, 0, 0, &tm); // sleep 
    }

    if (i == SECONDS_TO_WAIT * 100) {
        printf("Server not found after waiting %d seconds.\n", SECONDS_TO_WAIT);
        return -1; // timeout for server
    } 

    return 0;
}
