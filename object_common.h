#ifndef  __OBJECT_COMMON_H
#define __OBJECT_COMMON_H

extern void transfer(struct object *, struct object *);
extern struct object *duplex_obj(struct object *);
extern void com_transfer(struct object *);

#endif
