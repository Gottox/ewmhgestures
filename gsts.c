/*
 * gsts.c
 * Copyright (C) 2016 tox <tox@rootkit>
 *
 * Distributed under terms of the MIT license.
 */

#include "gsts.h"



static void
close_restricted(int fd, void *user_data)
{
	close(fd);
}

static int
open_restricted(const char *path, int flags, void *user_data)
{
	int fd;

	if ((fd = open(path, flags)) < 0)
		return -errno;

	return fd;
}

static const struct libinput_interface interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
};

static void
log_handler(struct libinput *li,
		enum libinput_log_priority priority,
		const char *format,
		va_list args)
{
	vprintf(format, args);
}

static int
open_udev(struct gsts *ctx) {
	if(!(ctx->udev = udev_new())) {
		return -1;
	}
	if(!(ctx->li = libinput_udev_create_context(&interface, ctx, ctx->udev))) {
		udev_unref(ctx->udev);
		return -1;
	}
	if (ctx->debug) {
		libinput_log_set_handler(ctx->li, log_handler);
		libinput_log_set_priority(ctx->li, LIBINPUT_LOG_PRIORITY_DEBUG);
	}
	if (libinput_udev_assign_seat(ctx->li, "seat0")) {
		libinput_unref(ctx->li);
		udev_unref(ctx->udev);
		return -1;
	}

	return 0;
}

int
gsts_init(struct gsts *ctx) {
	if(open_udev(ctx))
		return -1;

	ctx->fds.fd = libinput_get_fd(ctx->li);
	ctx->fds.events = POLLIN;
	ctx->fds.revents = 0;
	return 0;
}

struct libinput_event *
gsts_next_event(struct gsts *ctx) {
	struct libinput_event *ev;

	while(!(ev = libinput_get_event(ctx->li))) {
		if (poll(&ctx->fds, 1, -1) < 0) {
			return NULL;
		}

		if (libinput_dispatch(ctx->li)) {
			return NULL;
		}
	}

	return ev;
}

void
gsts_close(struct gsts *ctx) {
	libinput_unref(ctx->li);
	udev_unref(ctx->udev);
}
