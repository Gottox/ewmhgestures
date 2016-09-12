#include <stdlib.h>
#include <stdio.h>
#include <libinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ewmh.h"

int
main (int argc, char *argv[]) {
	struct ewmh ewmh;
	Display *dpy = XOpenDisplay(NULL);
	ewmh_init(&ewmh, dpy);
	ewmh_set_desktop(&ewmh, 10);
	ewmh_close(&ewmh);
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
