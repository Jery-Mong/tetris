#include <stdlib.h>
#include <string.h>
#include "cube.h"
#include "object_common.h"

void square_change_to_next(struct object *square)
{
	return;	
}	
void square_init(struct object *square)
{
	int i;	
	struct segment *seg;
	if (square->base_type != SQUARE)
		return;

	square->seg_cnt = 2;
	if (square->seg_head == NULL) {
		square->seg_head = (struct segment *)malloc(sizeof(struct segment) * square->seg_cnt);
		pthread_mutex_init(&square->lock, NULL);
	}
	
	square->status		    = 0;
	square->seg_head->brick_cnt = 2;
	square->seg_head->dir	    = HORIZONTAL;
	square->seg_head->y	    = -1;
	square->seg_head->x	    = rand() % (NET_SIZE_X - 1);
 	
	for (i = 1, seg = square->seg_head + 1; i < square->seg_cnt; i++, seg += 1) {
		memcpy(seg, square->seg_head, sizeof(struct segment));
		seg->y += i;
	}

}
void stick_change_to_next(struct object *stick)
{
	switch(stick->status) {
	case 0 :
		stick->seg_head->dir = VERTICAL;
		stick->seg_head->y -= 3;
		stick->seg_head->x += 1;
		stick->status = 1;	
		break;
	case 1:
		stick->seg_head->dir = HORIZONTAL;
		stick->seg_head->y += 3;
		stick->seg_head->x -= 1;
		stick->status = 0;
		break;
	default:
		break;
	}	
}

void stick_init(struct object *stick)
{
	if (stick->base_type != STICK)
		return;

	stick->seg_cnt = 1;
	if (stick->seg_head == NULL) {
		stick->seg_head = (struct segment *)malloc(sizeof(struct segment) * stick->seg_cnt);
		pthread_mutex_init(&stick->lock, NULL);	
	}

	stick->status		   = 1;
	stick->seg_head->brick_cnt = 4;
	stick->seg_head->dir	   = VERTICAL;
	stick->seg_head->y	   = -3;
	stick->seg_head->x	   = rand() % (NET_SIZE_X - 4);
	
	
	for (int i = rand() % 2; i > 0; i--)
		stick->change_to_next(stick);
}
void hook_init(struct object *hook)
{
	if (hook->base_type != HOOK)
		return;
	
	hook->seg_cnt = 2;
	if (hook->seg_head == NULL) {
		hook->seg_head = (struct segment *)malloc(sizeof(struct segment) * hook->seg_cnt);
		pthread_mutex_init(&hook->lock, NULL);
	}
	
	struct segment *seg = hook->seg_head;

	hook->status   = 0;
	seg->brick_cnt = 2;
	seg->dir       = HORIZONTAL;
	seg->y	       = -1;
	seg->x	       = rand() % (NET_SIZE_X - 2);


	(seg + 1)->dir       = VERTICAL;
	(seg + 1)->brick_cnt = 2;
	(seg + 1)->y	     = seg->y + 1;
	(seg + 1)->x	     = seg->x + 1;

	int i = rand() % 5;
	for (; i > 0; i--)
		hook->change_to_next(hook);
	
}

void hook_change_to_next(struct object *obj)
{
	struct segment *seg = obj->seg_head;
	switch (obj->status) {
	case 0:
		seg->x += 2;
		seg->dir = VERTICAL;
		
		seg++;
		seg->x -= 1;
		seg->dir = HORIZONTAL;
		
		obj->status = 1;
		break;
	case 1:
		seg->x -= 1;
		seg->y += 2;
		seg->dir = HORIZONTAL;

		seg++;
		seg->x += 1;
		seg->y -= 1;
		seg->dir = VERTICAL;
		
		obj->status = 2;
		break;
	case 2:
		seg->x -= 1;
		seg->y -= 1;
		seg->dir = VERTICAL;

		seg++;
		seg->y += 1;
		seg->dir = HORIZONTAL;
		
		obj->status = 3;
		break;
	case 3:
		seg->y -= 1;
		seg->dir = HORIZONTAL;

		seg++;
		seg->dir = VERTICAL;
		obj->status = 0;
		break;
	default:
		break;
	}
}

void tu_change_to_next(struct object *obj)
{
	struct segment * seg = obj->seg_head;
	switch (obj->status) {
	case 0:
		seg->x += 1;
		seg->y += 1;
		
		seg++;
		seg->x += 1;
		seg->y -= 1;
		seg->dir = VERTICAL;
		obj->status = 1;
		break;
	case 1:
		seg->x -= 1;
		seg->y += 1;


		seg++;
		seg->x -= 1;
		seg->y += 1;
		seg->dir = HORIZONTAL;
		obj->status = 2;
		break;
	case 2:
		seg->x -= 1;
		seg->y -= 1;

		seg++;
		seg->x += 1;
		seg->y -= 1;
		seg->dir = VERTICAL;
		
		obj->status = 3;
		break;
	case 3:
		seg->x += 1;
		seg->y -= 1;


		seg++;
		seg->x -= 1;
		seg->y += 1;
		seg->dir = HORIZONTAL;
		obj->status = 0;
		break;
	default:
		break;
	}
}
void tu_init(struct object *tu)
{
	if (tu->base_type != TU)
		return;
	
	tu->seg_cnt = 2;
	if (tu->seg_head == NULL) {
		tu->seg_head = (struct segment *)malloc(sizeof(struct segment) * tu->seg_cnt);
		pthread_mutex_init(&tu->lock, NULL);
	}
	
	struct segment *seg = tu->seg_head;

	tu->status   = 0;
	seg->brick_cnt = 1;
	seg->dir       = HORIZONTAL;
	seg->y	       = -1;
	seg->x	       = rand() % (NET_SIZE_X - 2);


	(seg + 1)->dir       = HORIZONTAL;
	(seg + 1)->brick_cnt = 3;
	(seg + 1)->x	     = seg->x - 1;
	(seg + 1)->y	     = seg->y + 1;

	int i = rand() % 5;
	for (; i > 0; i--)
		tu->change_to_next(tu);
	
}

void rhook_init(struct object *rhook)
{
	if (rhook->base_type != RHOOK)
		return;
	
	rhook->seg_cnt = 2;
	if (rhook->seg_head == NULL) {
		rhook->seg_head = (struct segment *)malloc(sizeof(struct segment) * rhook->seg_cnt);
		pthread_mutex_init(&rhook->lock, NULL);
	}
	
	struct segment *seg = rhook->seg_head;

	rhook->status   = 0;
	seg->brick_cnt = 2;
	seg->dir       = HORIZONTAL;
	seg->y	       = -1;
	seg->x	       = rand() % (NET_SIZE_X - 2);


	(seg + 1)->y	     = seg->y + 1;
	(seg + 1)->x         = seg->x;
	(seg + 1)->dir       = VERTICAL;
	(seg + 1)->brick_cnt = 2;

	int i = rand() % 5;
	for (; i > 0; i--)
		rhook->change_to_next(rhook);
	
}

void rhook_change_to_next(struct object *obj)
{
	struct segment *seg = obj->seg_head;
	switch (obj->status) {
	case 0:
		seg->x -= 1;
		seg->dir = VERTICAL;
		
		seg++;
		seg->dir = HORIZONTAL;
		
		obj->status = 1;
		break;
	case 1:
		seg->y += 2;
		seg->dir = HORIZONTAL;

		seg++;
		seg->y -= 1;
		seg->dir = VERTICAL;
		
		obj->status = 2;
		break;
	case 2:
		seg->x += 2;
		seg->y -= 1;
		seg->dir = VERTICAL;

		seg++;
		seg->x -= 1;
		seg->y += 1;
		seg->dir = HORIZONTAL;
		
		obj->status = 3;
		break;
	case 3:
		seg->y -= 1;
		seg->x -= 1;
		seg->dir = HORIZONTAL;

		seg++;
		//	seg->y += 1;
		seg->x += 1;
		seg->dir = VERTICAL;
		obj->status = 0;
		break;
	default:
		break;
	}
}

void lightning_init(struct object *lightning)
{
	if (lightning->base_type != LIGHTNING)
		return;
	
	lightning->seg_cnt = 2;
	if (lightning->seg_head == NULL) {
		lightning->seg_head = (struct segment *)malloc(sizeof(struct segment) * lightning->seg_cnt);
		pthread_mutex_init(&lightning->lock, NULL);
	}
	
	struct segment *seg = lightning->seg_head;

	lightning->status   = 0;
	seg->brick_cnt = 2;
	seg->dir       = VERTICAL;
	seg->y	       = -1;
	seg->x	       = rand() % (NET_SIZE_X - 1);


	(seg + 1)->y	     = seg->y + 1;
	(seg + 1)->x         = seg->x + 1;
	(seg + 1)->dir       = VERTICAL;
	(seg + 1)->brick_cnt = 2;

	int i = rand() % 2;
	for (; i > 0; i--)
		lightning->change_to_next(lightning);
	
}

void lightning_change_to_next(struct object *obj)
{
	struct segment *seg = obj->seg_head;
	switch (obj->status) {
	case 0:
		seg->x += 1;
		seg->dir = HORIZONTAL;
		
		seg++;
		seg->x -= 1;
		seg->dir = HORIZONTAL;
		
		obj->status = 1;
		break;
	case 1:
		seg->x -= 1;
		seg->dir = VERTICAL;

		seg++;
		seg->x += 1;
		seg->dir = VERTICAL;
		
		obj->status = 0;
		break;
	default:
		break;
	}
}
void rlightning_init(struct object *rlightning)
{
	if (rlightning->base_type != RLIGHTNING)
		return;
	
	rlightning->seg_cnt = 2;
	if (rlightning->seg_head == NULL) {
		rlightning->seg_head = (struct segment *)malloc(sizeof(struct segment) * rlightning->seg_cnt);
		pthread_mutex_init(&rlightning->lock, NULL);
	}
	
	struct segment *seg = rlightning->seg_head;

	rlightning->status   = 0;
	seg->brick_cnt = 2;
	seg->dir       = VERTICAL;
	seg->y	       = -1;
	seg->x	       = rand() % (NET_SIZE_X - 1);


	(seg + 1)->y	     = seg->y + 1;
	(seg + 1)->x         = seg->x - 1;
	(seg + 1)->dir       = VERTICAL;
	(seg + 1)->brick_cnt = 2;

	int i = rand() % 2;
	for (; i > 0; i--)
		rlightning->change_to_next(rlightning);
	
}

void rlightning_change_to_next(struct object *obj)
{
	struct segment *seg = obj->seg_head;
	switch (obj->status) {
	case 0:
		seg->x -= 1;
		seg->dir = HORIZONTAL;
		
		seg++;
		seg->x += 1;
		seg->dir = HORIZONTAL;
		
		obj->status = 1;
		break;
	case 1:
		seg->x += 1;
		seg->dir = VERTICAL;

		seg++;
		seg->x -= 1;
		seg->dir = VERTICAL;
		
		obj->status = 0;
		break;
	default:
		break;
	}
}
