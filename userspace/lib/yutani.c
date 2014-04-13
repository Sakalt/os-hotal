#include <string.h>
#include <stdlib.h>
#include <syscall.h>

#include "yutani.h"
#include "pex.h"
#include "graphics.h"
#include "kbd.h"
#include "mouse.h"

yutani_msg_t * yutani_wait_for(yutani_t * y, uint32_t type) {
	do {
		yutani_msg_t * out;
		size_t size;
		{
			char tmp[MAX_PACKET_SIZE];
			size = pex_recv(y->sock, tmp);
			out = malloc(size);
			memcpy(out, tmp, size);
		}

		if (out->type == type) {
			return out;
		} else {
			/* XXX: Add other messages to a queue to be handled later. */
			free(out);
		}
	} while (1); /* XXX: (!y->abort) */
}

yutani_msg_t * yutani_poll(yutani_t * y) {
	yutani_msg_t * out;
	size_t size;
	{
		char tmp[MAX_PACKET_SIZE];
		size = pex_recv(y->sock, tmp);
		out = malloc(size);
		memcpy(out, tmp, size);
	}

	return out;
}

yutani_msg_t * yutani_msg_build_hello(void) {
	size_t s = sizeof(struct yutani_message);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_HELLO;
	msg->size  = s;

	return msg;
}

yutani_msg_t * yutani_msg_build_flip(yutani_wid_t wid) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_flip);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_FLIP;
	msg->size  = s;

	struct yutani_msg_flip * mw = (void *)msg->data;

	mw->wid = wid;

	return msg;
}

yutani_msg_t * yutani_msg_build_welcome(uint32_t width, uint32_t height) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_welcome);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WELCOME;
	msg->size  = s;

	struct yutani_msg_welcome * mw = (void *)msg->data;

	mw->display_width = width;
	mw->display_height = height;

	return msg;
}

yutani_msg_t * yutani_msg_build_window_new(uint32_t width, uint32_t height) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_window_new);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WINDOW_NEW;
	msg->size  = s;

	struct yutani_msg_window_new * mw = (void *)msg->data;

	mw->width = width;
	mw->height = height;

	return msg;
}

yutani_msg_t * yutani_msg_build_window_init(yutani_wid_t wid, uint32_t width, uint32_t height, uint32_t bufid) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_window_init);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WINDOW_INIT;
	msg->size  = s;

	struct yutani_msg_window_init * mw = (void *)msg->data;

	mw->wid = wid;
	mw->width = width;
	mw->height = height;
	mw->bufid = bufid;

	return msg;
}

yutani_msg_t * yutani_msg_build_window_close(yutani_wid_t wid) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_window_close);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WINDOW_CLOSE;
	msg->size  = s;

	struct yutani_msg_flip * mw = (void *)msg->data;

	mw->wid = wid;

	return msg;
}

yutani_msg_t * yutani_msg_build_key_event(yutani_wid_t wid, key_event_t * event) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_key_event);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_KEY_EVENT;
	msg->size  = s;

	struct yutani_msg_key_event * mw = (void *)msg->data;

	mw->wid = wid;
	memcpy(&mw->event, event, sizeof(key_event_t));

	return msg;
}

yutani_msg_t * yutani_msg_build_mouse_event(yutani_wid_t wid, mouse_device_packet_t * event) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_mouse_event);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_MOUSE_EVENT;
	msg->size  = s;

	struct yutani_msg_mouse_event * mw = (void *)msg->data;

	mw->wid = wid;
	memcpy(&mw->event, event, sizeof(mouse_device_packet_t));

	return msg;
}

yutani_msg_t * yutani_msg_build_window_move(yutani_wid_t wid, int32_t x, int32_t y) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_window_move);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WINDOW_MOVE;
	msg->size  = s;

	struct yutani_msg_window_move * mw = (void *)msg->data;

	mw->wid = wid;
	mw->x = x;
	mw->y = y;

	return msg;
}

yutani_msg_t * yutani_msg_build_window_stack(yutani_wid_t wid, int z) {
	size_t s = sizeof(struct yutani_message) + sizeof(struct yutani_msg_window_stack);
	yutani_msg_t * msg = malloc(s);

	msg->magic = YUTANI_MSG__MAGIC;
	msg->type  = YUTANI_MSG_WINDOW_STACK;
	msg->size  = s;

	struct yutani_msg_window_stack * mw = (void *)msg->data;

	mw->wid = wid;
	mw->z = z;

	return msg;
}

int yutani_msg_send(yutani_t * y, yutani_msg_t * msg) {
	return pex_reply(y->sock, msg->size, (char *)msg);
}

yutani_t * yutani_context_create(FILE * socket) {
	yutani_t * out = malloc(sizeof(yutani_t));

	out->sock = socket;
	out->display_width  = 0;
	out->display_height = 0;
	return out;
}

yutani_t * yutani_init(void) {
	/* XXX: Display, etc? */
	if (!getenv("DISPLAY")) {
		return NULL;
	}
	FILE * c = pex_connect(getenv("DISPLAY"));

	if (!c) return NULL; /* Connection failed. */

	fprintf(stderr, "[yutani-client] Sending hello...\n");
	yutani_t * y = yutani_context_create(c);
	yutani_msg_t * m = yutani_msg_build_hello();
	int result = yutani_msg_send(y, m);
	free(m);

	fprintf(stderr, "[yutani-client] Waiting for welcome...\n");
	m = yutani_wait_for(y, YUTANI_MSG_WELCOME);
	struct yutani_msg_welcome * mw = (void *)&m->data;
	y->display_width = mw->display_width;
	y->display_height = mw->display_height;
	free(m);

	fprintf(stderr, "[yutani-client] Received welcome with server size of: %dx%d\n", y->display_width, y->display_height);

	return y;
}

yutani_window_t * yutani_window_create(yutani_t * y, int width, int height) {
	yutani_window_t * win = malloc(sizeof(yutani_window_t));

	yutani_msg_t * m = yutani_msg_build_window_new(width, height);
	int result = yutani_msg_send(y, m);
	free(m);

	fprintf(stderr, "[yutani-client] Waiting for window response...\n");
	m = yutani_wait_for(y, YUTANI_MSG_WINDOW_INIT);
	struct yutani_msg_window_init * mw = (void *)&m->data;
	fprintf(stderr, "[yutani-client] Received response, buffer ID is %d\n", mw->bufid);

	win->width = mw->width;
	win->height = mw->height;
	win->bufid = mw->bufid;
	win->wid = mw->wid;

	char key[1024];
	YUTANI_SHMKEY(key, 1024, win);

	size_t size = (width * height * 4);
	win->buffer = (uint8_t *)syscall_shm_obtain(key, &size);
	return win;
}

void yutani_flip(yutani_t * y, yutani_window_t * win) {
	yutani_msg_t * m = yutani_msg_build_flip(win->wid);
	int result = yutani_msg_send(y, m);
	free(m);
}

void yutani_close(yutani_t * y, yutani_window_t * win) {
	yutani_msg_t * m = yutani_msg_build_window_close(win->wid);
	int result = yutani_msg_send(y, m);
	free(m);
}

void yutani_window_move(yutani_t * yctx, yutani_window_t * window, int x, int y) {
	yutani_msg_t * m = yutani_msg_build_window_move(window->wid, x, y);
	int reuslt = yutani_msg_send(yctx, m);
	free(m);
}

void yutani_set_stack(yutani_t * yctx, yutani_window_t * window, int z) {
	yutani_msg_t * m = yutani_msg_build_window_stack(window->wid, z);
	int reuslt = yutani_msg_send(yctx, m);
	free(m);
}

gfx_context_t * init_graphics_yutani(yutani_window_t * window) {
	gfx_context_t * out = malloc(sizeof(gfx_context_t));
	out->width  = window->width;
	out->height = window->height;
	out->depth  = 32;
	out->size   = GFX_H(out) * GFX_W(out) * GFX_B(out);
	out->buffer = window->buffer;
	out->backbuffer = out->buffer;
	return out;
}

gfx_context_t *  init_graphics_yutani_double_buffer(yutani_window_t * window) {
	gfx_context_t * out = init_graphics_yutani(window);
	out->backbuffer = malloc(sizeof(uint32_t) * GFX_W(out) * GFX_H(out));
	return out;
}

void reinit_graphics_yutani(gfx_context_t * out, yutani_window_t * window) {
	out->width  = window->width;
	out->height = window->height;
	out->depth  = 32;
	out->size   = GFX_H(out) * GFX_W(out) * GFX_B(out);
	if (out->buffer == out->backbuffer) {
		out->buffer = window->buffer;
		out->backbuffer = out->buffer;
	} else {
		out->buffer = window->buffer;
		out->backbuffer = realloc(out->backbuffer, sizeof(uint32_t) * GFX_W(out) * GFX_H(out));
	}
}

