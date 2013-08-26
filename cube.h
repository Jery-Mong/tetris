#ifndef __CUBE_H
#define __CUBE_H

#include <pthread.h>

#define BRICK_X 4
#define BRICK_Y 2

#define WINDOW_X 48
#define WINDOW_Y 46

#define NET_SIZE_X (WINDOW_X / BRICK_X)
#define NET_SIZE_Y (WINDOW_Y / BRICK_Y)
#define DELAY_NORMAL	500000
#define DELAY_FAST	10000

enum key {
	CTRL_UP,
	CTRL_DOWN,
	CTRL_BACKSPACE,
	CTRL_ENTER,
	CTRL_LEFT,
	CTRL_RIGHT,
	CTRL_OTHER
};
enum seg_dir {
	HORIZONTAL,
	VERTICAL
};
enum obj_type {
	SQUARE,
	STICK,
	HOOK,
	TU,
	RHOOK,
	LIGHTNING,
	RLIGHTNING,
};
enum position {
	POSITION_FREE,
	POSITION_STOP,
	POSITION_TERMINATED
};

struct segment {
	int dir;
	int x, y;
	int brick_cnt;
};

struct object {
	int base_type;
	
	int status;
	struct segment *seg_head;
	int seg_cnt;
	void (*init)(struct object *obj);
	void (*change_to_next)(struct object *obj);
	pthread_mutex_t lock;
};

extern int check_position_h(struct object *, int);
extern int check_position_v(struct object *);
extern void move_steps(struct object *, int , int);

#endif
