/* Copyright (C) 2004,2005 David Antliff <dave.antliff@paradise.net.nz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 * Version 0.4 20050719 - added manual screen selection (thanks Vita Batrla)
 * Version 0.3 20050719 - renamed to 'switchscreen'
 * Version 0.2 20050128 - added support for Window Focus
 * Version 0.1 20041014
 *
 * This program allows you to move the mouse cursor between non-xinerama screens
 * in a dual head configuration.
 *
 * Compile with:
 *   gcc -g switchscreen.c -o switchscreen -L /usr/X11R6/lib -lX11 -lXtst -lXext
 *
 * Stores previous screen's pointer location in ~/.pointer-<screen>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>


#define REMEMBER_FILE ".pointer"

/******************************************************************************
FUNCTION    : save
DESCRIPTION : Save the cursor coordinates and focused screen to the RememberFile
******************************************************************************/
void save(int screen, int x, int y, Window focus)
{
    FILE * f;
    char acFile[100];
    char acData[100];
    char * home;

    home = getenv("HOME");
    snprintf(acFile, 100, "%s/%s-%d", home, REMEMBER_FILE, screen);
    f = fopen(acFile, "w");

    if (f != NULL)
    {
        fprintf(stderr, "Saving %s\n", acFile);
        snprintf(acData, 100, "%d %d %d\n", x, y, (int)focus);

        if (fputs(acData, f) < 0)
        {
            perror("fputs");
        }

        fclose(f);
    }
    else
    {
        perror("fopen");
    }
}


/******************************************************************************
FUNCTION    : load
DESCRIPTION : Load the cursor coordinates and focused screen from the RememberFile
******************************************************************************/
void load(int screen, int * px, int * py, Window *pfocus)
{
    FILE * f;
    char acFile[100];
    char acData[100];
    char * home;
    int x = 0;
    int y = 0;
    int focus = 0;

    home = getenv("HOME");
    snprintf(acFile, 100, "%s/%s-%d", home, REMEMBER_FILE, screen);
    f = fopen(acFile, "r");

    if (f != NULL)
    {
        fprintf(stderr, "Loading %s\n", acFile);
        if (fgets(acData, 100, f) == NULL)
        {
            perror("fgets");
        }
        else
        {
            sscanf(acData, "%d %d %d\n", &x, &y, &focus);
        }

        fclose(f);
    }

    *px = x;
    *py = y;
    *pfocus = focus;
}


/******************************************************************************
FUNCTION    : main
DESCRIPTION :
******************************************************************************/
int main(int argc, char ** argv)
{
    Display * display;
    int majorOpcode, firstEvent, firstError;
	char * displayName;
    int thisScreen;
    int otherScreen = -1;

    if (argc == 2)
    {
        otherScreen = atol(argv[1]);
    }
    else if (argc != 1)
    {
        fprintf(stderr, "Usage: switchscreen [other screen number]\n");
        fprintf(stderr, "  By default, the other screen is 1 - this screen.\n");
        exit(1);
    }

    displayName = getenv("DISPLAY");
    if (displayName == NULL)
    {
        fprintf(stderr, "DISPLAY is not set\n");
        exit(1);
    }

    fprintf(stderr, "Opening display %s\n", displayName);
    display = XOpenDisplay(displayName);


    if (!XQueryExtension(display,
                         XTestExtensionName,
                         &majorOpcode,
                         &firstEvent,
                         &firstError))
    {
        fprintf(stderr, "XTEST extension not available\n");
        return (1);
    }


    thisScreen = DefaultScreen(display);
    if (otherScreen == -1)
    {
        otherScreen = 1 - DefaultScreen(display);
    }

    if (otherScreen != thisScreen)
    {
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask;
        Window root, child;
        Window focus = 0;
        int revert_to = 0;


        // get current mouse coordinates
        XQueryPointer(display,
                      RootWindow(display, thisScreen),
                      &root,
                      &child,
                      &root_x,
                      &root_y,
                      &win_x,
                      &win_y,
                      &mask);

        // get ID of window with focus
        XGetInputFocus(display,
                       &focus,
                       &revert_to);

        fprintf(stderr, "Window ID is %x, focus is %x\n", (int)focus, revert_to);


        // save to .pointer<screen>
        save(thisScreen, root_x, root_y, focus);

        // load from .pointer<otherScreen>
        load(otherScreen, &root_x, &root_y, &focus);

        fprintf(stderr, "Moving to screen %d, x = %d, y = %d\n", otherScreen, root_x, root_y);

        // move mouse to other screen
        XTestFakeMotionEvent(display,
                             otherScreen,
                             root_x,
                             root_y,
                             CurrentTime);

        // set ID of window with focus
        // note that the window might not be there any more, so be prepared to fail
        XSetInputFocus(display,
                       focus,
                       1,   // revert_to
                       CurrentTime);
    }

    XCloseDisplay(display);

    return (0);
}

/*****************************************************************************
*
* $Log: switchscreen.c,v $
* Revision 1.4  2005/07/19 02:03:14  david
* version 0.4 - added manual screen selection (thanks Vita Batrla)
*
* Revision 1.3  2005/07/19 01:47:41  david
* version 0.3 - added focus support
*
*
*****************************************************************************/
