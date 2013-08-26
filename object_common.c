#include <stdlib.h>
#include <string.h>
#include "cube.h"

struct object *duplex_obj(struct object *obj)
{
	struct object *dup_obj;

	dup_obj = (struct object *)malloc(sizeof(struct object));
	memcpy(dup_obj, obj, sizeof(struct object));
	
	dup_obj->seg_head = (struct segment *)malloc(sizeof(struct segment) * obj->seg_cnt);
	
	memcpy(dup_obj->seg_head, obj->seg_head, sizeof(struct segment) * obj->seg_cnt);
	return dup_obj;
}
static int check_allow_trans(struct object *obj)
{	
	move_steps(obj, 1,  CTRL_LEFT);
	if (check_position_h(obj, CTRL_RIGHT) == POSITION_STOP)
		return POSITION_STOP;
	
	move_steps(obj, 2, CTRL_RIGHT);
	if (check_position_h(obj, CTRL_LEFT) == POSITION_STOP)
		return POSITION_STOP;
	
	move_steps(obj, 1, CTRL_LEFT);
	move_steps(obj, 1, CTRL_UP);
	if (check_position_v(obj) == POSITION_STOP)
		return POSITION_STOP;
	
	move_steps(obj, 1, CTRL_DOWN);
	return POSITION_FREE;
}
void transfer(struct object *orig, struct object *dup_obj)
{
	if (check_allow_trans(dup_obj) == POSITION_FREE) {
		memcpy(orig, dup_obj, sizeof(struct object));
		memcpy(orig->seg_head, dup_obj->seg_head, sizeof(struct segment) * orig->seg_cnt);
	}
}
void com_transfer(struct object *obj)
{
	struct object *dup_obj = duplex_obj(obj);
	obj->change_to_next(dup_obj);
	transfer(obj, dup_obj);
	free(dup_obj);
}
