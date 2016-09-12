/*
 * ewmh.c
 * Copyright (C) 2016 tox <tox@rootkit>
 *
 * Distributed under terms of the MIT license.
 */

#include "ewmh.h"
#define MAX_PROPVAL_LEN 4096

int
ewmh_init(struct ewmh *ctx, Display *display) {
	ctx->display = display;
	ctx->screen = DefaultScreen(ctx->display);
	ctx->root = RootWindow(ctx->display, ctx->screen);

	/* ATOMS */
	ctx->atoms[NetCurrentDesktop] = XInternAtom(ctx->display, "_NET_CURRENT_DESKTOP", False);

	return 0;
}

int
ewmh_set_desktop(struct ewmh *ctx, long desktop) {
	int rv;
	XEvent ev = {
		.xclient = {
			.type = ClientMessage,
			.serial = 0,
			.send_event = True,
			.message_type = ctx->atoms[NetCurrentDesktop],
			.window = ctx->root,
			.format = 32,
			.data.l[0] = desktop,
		}
	};

	rv = XSendEvent(ctx->display, ctx->root, False,
			SubstructureRedirectMask | SubstructureNotifyMask, &ev) != Success;
	XFlush(ctx->display);
	return rv;
}

long
ewmh_get_desktop(struct ewmh *ctx) {
	Atom rtype;
	int rformat;
	unsigned long nitems;
	unsigned long bytesafter;
	unsigned char *rprop;


	if (XGetWindowProperty(ctx->display, ctx->root,
			ctx->atoms[NetCurrentDesktop], 0, MAX_PROPVAL_LEN / 4,
			False, XA_CARDINAL, &rtype, &rformat, &nitems, &bytesafter, &rprop) != Success) {
		return -1;
	}

	return (long) *rprop;
}

void
ewmh_close(struct ewmh *ctx) {
}
