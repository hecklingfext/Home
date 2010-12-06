/*
 * switchscreen - switches pointer between screens of an X server
 * Copyright (C) 2003 Sampo Niskanen <sampo.niskanen@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define VERSION "0.1.1"

#define ACTION_PRINTSCREEN 1
#define ACTION_PRINTCOORDS 2
#define ACTION_MOVESCREEN  3


void help(void) {
	printf("Usage:  switchscreen [options] <screen>\n");
	printf("\n");
	printf("<screen> is the screen number to switch to.  Options are the following:\n");
	printf("\n");
	printf("   -p      --print      Print current screen number and pointer coordinates.\n");
	printf("   -c x,y  --coord x,y  Set pointer to coordinates (x,y).\n");
	printf("   -q      --quiet      Quiet operation.\n");
	printf("   -h      --help       Show this help screen.\n");
	printf("   -V      --version    Show program version.\n");
	printf("\n");
	return;
}



int main(int argc, char *argv[]) {
	Display *dpy;
	Screen *scr;
	Window wnd;
	int c;
	int screen=-1;
	int x=-1, y=-1;
	int quiet=0;
	int print=0;

	Window wtmp;
	int itmp;


	/* Parse command line */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"print", 0, 0, 'p'},
			{"coord", 1, 0, 'c'},
			{"quiet", 0, 0, 'q'},

			{"help", 0, 0, 'h'},
			{"version", 0, 0, 'V'},
			{0,0,0,0}
		};
		
		c=getopt_long(argc,argv,"pPmc:qhV",long_options,&option_index);
		if (c==-1)
			break;
		
		switch (c) {
		case 0:
			break;

		case 'c':
			if (sscanf(optarg,"%d,%d",&x,&y)!=2) {
				fprintf(stderr,"Bad argument for -c: %s\n"
					"(Correct format is 'x,y'.)\n",
					optarg);
				exit(1);
			}
			break;

		case 'p':
		case 'P':  /* Compatibility, may disappear in future */
			print=1;
			break;

		case 'q':
			quiet=1;
			break;

		case 'h':
			help();
			exit(0);

		case 'V':
			printf("switchscreen version " VERSION "\n");
			exit(0);

		default:
			fprintf(stderr,"Use '%s -h' for help.\n",argv[0]);
			exit(1);

		}
	}


	/* Check remaining command line options */
	if (optind < argc) {
		if (optind+1 < argc) {
			fprintf(stderr,"Too many arguments for format.\n");
			fprintf(stderr,"Use '%s -h' for help.\n",argv[0]);
			exit(1);
		}
		if (sscanf(argv[optind],"%d",&screen)<1) {
			fprintf(stderr,"Bad argument:  %s\n",argv[optind]);
			fprintf(stderr,"Use '%s -h' for help.\n",argv[0]);
			exit(1);
		}
	}


	/* No necessary options given. */
	if (screen<0 && (x<0 || y<0) && !print) {
		help();
		exit(1);
	}


	/* Open display */
	dpy = XOpenDisplay(NULL);
	if (dpy==NULL) {
		fprintf(stderr,"%s:  Cannot open display.\n",argv[0]);
		exit(1);
	}


	/* Get screen number if not specified (or if to be printed) */
	if ((screen<0) || print) {
		scr=XDefaultScreenOfDisplay(dpy);
		if (scr==NULL) {
			fprintf(stderr,"XDefaultScreenOfDisplay "
				"returned NULL!\n");
			exit(1);
		}
		screen=XScreenNumberOfScreen(scr);
	}


	/* Only print position */
	if (print) {
		wnd=XDefaultRootWindow(dpy);
		XQueryPointer(dpy,wnd,&wtmp,&wtmp,&x,&y,&itmp,&itmp,
				      (unsigned int*)&itmp);
		
		printf("Screen: %d  Coordinates: %d,%d\n",screen,x,y);
		exit(0);
	}


	/* Set cursor to middle of screen */
	if (x<0 || y<0) {
		scr=ScreenOfDisplay(dpy,screen);
		if (scr==NULL) {
			fprintf(stderr,"XScreensOfDisplay returned NULL!\n");
			exit(1);
		}
		x=XWidthOfScreen(scr)/2;
		y=XHeightOfScreen(scr)/2;
	}


	if (!quiet) {
		if (screen==-1)
			printf("Setting coordinates %d,%d.\n",x,y);
		else
			printf("Setting Screen %d at coordinates %d,%d.\n",
			       screen,x,y);
	}

	wnd = RootWindow(dpy, screen);

	XSetInputFocus(dpy, wnd, RevertToNone, CurrentTime);
	XFlush(dpy);
	XGrabPointer(dpy, wnd, False, 0, GrabModeAsync, GrabModeAsync, None,
			None, CurrentTime);
	XWarpPointer(dpy, None, wnd, 0, 0, 0, 0, x, y);
	XUngrabPointer(dpy, CurrentTime);

	XCloseDisplay(dpy);
	return 0;
}
