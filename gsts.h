/*
 * gestures.h
 * Copyright (C) 2016 tox <tox@rootkit>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef GESTURES_H
#define GESTURES_H
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <libinput.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct gsts {
	struct libinput *li;
	struct udev *udev;
	int debug;
	struct pollfd fds;
};

int gsts_init(struct gsts *ctx);
struct libinput_event * gsts_next_event(struct gsts *ctx);
void gsts_close(struct gsts *ctx);

#endif /* !GESTURES_H */
