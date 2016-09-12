/*
 * ewmh.h
 * Copyright (C) 2016 tox <tox@rootkit>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef EWMH_H
#define EWMH_H

#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>

/* EWMH atoms */
enum { NetCurrentDesktop, NetLast };

struct ewmh {
	Display *display;
	int screen;
	Window root;
	Atom atoms[NetLast];
};

int ewmh_init(struct ewmh *ctx, Display *display);
int ewmh_set_desktop(struct ewmh *ctx, long desktop);
long ewmh_get_desktop(struct ewmh *ctx);
void ewmh_close(struct ewmh *ctx);
#endif /* !EWMH_H */
