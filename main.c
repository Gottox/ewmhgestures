#include <stdlib.h>
#include <stdio.h>
#include <libinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <X11/extensions/XTest.h>


#include "ewmh.h"
#include "gsts.h"

#define THRESHOLD 30.

struct gesture_info {
	struct gsts gsts;
	struct ewmh ewmh;
	struct libinput_event *ev;
	Display *dpy;
	double x;
	double y;
};

static void handle_event(struct gesture_info *ctx, struct libinput_event *ev);
static void handle_swipe(struct gesture_info *ctx, struct libinput_event *ev);

static void
pressKey(struct gesture_info *ctx, Bool pressed, unsigned int key) {
	XTestFakeKeyEvent(ctx->dpy, key, pressed, 0);
	XFlush(ctx->dpy);
}

static void
handle_horizontal_swipe(struct gesture_info *ctx, struct libinput_event *ev) {
	struct libinput_event_gesture *t;

	unsigned int alt = XKeysymToKeycode(ctx->dpy, XK_Alt_L);
	unsigned int tab = XKeysymToKeycode(ctx->dpy, XK_Tab);
	unsigned int shift = XKeysymToKeycode(ctx->dpy, XK_Shift_L);

	pressKey(ctx, True, alt);
	do {
		switch(libinput_event_get_type(ev)) {
		case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
			t = libinput_event_get_gesture_event(ev);
			ctx->x += libinput_event_gesture_get_dx(t);
			while(fabs(ctx->x) > THRESHOLD) {
				if(ctx->x < 0)
					pressKey(ctx, True, shift);
				pressKey(ctx, True, tab);
				pressKey(ctx, False, tab);
				if(ctx->x < 0)
					pressKey(ctx, False, shift);
				ctx->x += ctx->x > 0 ? -THRESHOLD : THRESHOLD;
			}
			break;
		case LIBINPUT_EVENT_GESTURE_SWIPE_END:
			pressKey(ctx, False, alt);
			return;
		default:
			handle_event(ctx, ev);
		}
	} while((ev = gsts_next_event(&ctx->gsts)));
	pressKey(ctx, False, alt);
}

static void
handle_vertical_swipe(struct gesture_info *ctx, struct libinput_event *ev) {
	struct libinput_event_gesture *t = libinput_event_get_gesture_event(ev);
	int triggered = 0;
	int desktop;

	int fingers = libinput_event_gesture_get_finger_count(t);
	unsigned int super = XKeysymToKeycode(ctx->dpy, XK_Super_L);

	do {
		switch(libinput_event_get_type(ev)) {
		case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
			if(triggered)
				break;
			t = libinput_event_get_gesture_event(ev);
			ctx->y += libinput_event_gesture_get_dy(t);
			if(fabs(ctx->y) > THRESHOLD) {
				triggered = 1;
				if(fingers == 4) {
					pressKey(ctx, True, super);
					pressKey(ctx, False, super);
				} else {
					desktop = ewmh_get_desktop(&ctx->ewmh);
					ewmh_set_desktop(&ctx->ewmh, desktop - (ctx->y > 0 ? 1 : -1));
				}
			}
			break;
		case LIBINPUT_EVENT_GESTURE_SWIPE_END:
			return;
		default:
			handle_event(ctx, ev);
		}
	} while((ev = gsts_next_event(&ctx->gsts)));
}


static void
handle_swipe(struct gesture_info *ctx, struct libinput_event *ev) {
	double dx, dy;
	struct libinput_event_gesture *t;


	do {
		switch(libinput_event_get_type(ev)) {
		case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
			ctx->x = ctx->y = 0;
			break;
		case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
			t = libinput_event_get_gesture_event(ev);
			dx = libinput_event_gesture_get_dx(t);
			dy = libinput_event_gesture_get_dy(t);
			if(fabs(dx + ctx->x) > THRESHOLD) {
				handle_horizontal_swipe(ctx, ev);
				return;
			}
			else if(fabs(dy + ctx->y) > THRESHOLD) {
				handle_vertical_swipe(ctx, ev);
				return;
			}
			ctx->x += dx;
			ctx->y += dy;
			break;
		case LIBINPUT_EVENT_GESTURE_SWIPE_END:
			return;
		default:
			handle_event(ctx, ev);
		}
	} while((ev = gsts_next_event(&ctx->gsts)));
}

static void
handle_event(struct gesture_info *ctx, struct libinput_event *ev) {
	switch(libinput_event_get_type(ev)) {
	case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
		handle_swipe(ctx, ev);
		break;
	default:
		// empty
		break;
	}
}

int
main (int argc, char *argv[]) {
	struct gesture_info ctx = { { 0 } };
	struct libinput_event *ev;

	ctx.dpy = XOpenDisplay(NULL);
	ewmh_init(&ctx.ewmh, ctx.dpy);
	gsts_init(&ctx.gsts);

	while((ev = gsts_next_event(&ctx.gsts))) {
		handle_event(&ctx, ev);
	}

	gsts_close(&ctx.gsts);
	ewmh_close(&ctx.ewmh);
	XCloseDisplay(ctx.dpy);
	return EXIT_SUCCESS;
}
