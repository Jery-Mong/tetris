#ifndef __OBJECTS_H
#define __OBJECTS_H

extern void square_init(struct object *);
extern void square_change_to_next(struct object *);
extern void stick_init(struct object *);
extern void stick_change_to_next(struct object *);

extern void hook_init(struct object *);
extern void hook_change_to_next(struct object *);
extern void tu_init(struct object *);
extern void tu_change_to_next(struct object *);

extern void rhook_init(struct object *);
extern void rhook_change_to_next(struct object *);

extern void lightning_init(struct object *);
extern void lightning_change_to_next(struct object *);

extern void rlightning_init(struct object *);
extern void rlightning_change_to_next(struct object *);

#endif
