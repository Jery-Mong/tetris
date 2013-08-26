#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "cube.h"
#include "objects.h"
#include "own_signal.h"
#include "object_common.h"

char (*net)[NET_SIZE_X];
struct object *current;
WINDOW *game_erea;

pthread_t main_tid;
int pause_flag = 0;
int cut_of_point = NET_SIZE_Y;
unsigned int delay = DELAY_NORMAL;

struct object obj_vec[] = {
	[0] = {
		.base_type	= SQUARE,
		.seg_head	= NULL,
		.init		= square_init,
		.change_to_next = square_change_to_next,
	},[1] = {
		.base_type	= STICK,
		.seg_head	= NULL,
		.init		= stick_init,
		.change_to_next = stick_change_to_next,
	},[2] = {
		.base_type	= HOOK,
		.seg_head	= NULL,
		.init		= hook_init,
		.change_to_next = hook_change_to_next,		
	},[3] = {
		.base_type	= TU,
		.seg_head	= NULL,
		.init		= tu_init,
		.change_to_next = tu_change_to_next,		
	},[4] = {
		.base_type	= RHOOK,
		.seg_head	= NULL,
		.init		= rhook_init,
		.change_to_next = rhook_change_to_next,
	},[5] = {
		.base_type	= LIGHTNING,
		.seg_head	= NULL,
		.init		= lightning_init,
		.change_to_next = lightning_change_to_next,
	},[6] = {
		.base_type	= RLIGHTNING,
		.seg_head	= NULL,
		.init		= rlightning_init,
		.change_to_next = rlightning_change_to_next,
	}
};
unsigned int object_nr = sizeof(obj_vec) / sizeof(obj_vec[0]);

void paint_brick(int net_x, int net_y)
{
	int i, j;
	
	for(j	       = 0; j < BRICK_Y; j++) {
		wmove(game_erea, net_y* BRICK_Y + j, net_x * BRICK_X);
		for (i = 0; i < BRICK_X; i++)
			waddch(game_erea, ' ' | A_REVERSE);
	}
}
/*
void paint_brick(int x, int y)
{
	wmove(game_erea, y, x);
	waddch(game_erea, '*');
}
*/
void display_game_erea()
{
	int		 i;
	struct object	*obj = current;
	struct segment	*seg;

	wclear(game_erea);
	box(game_erea, '|', ' ');
	
	for (int y = cut_of_point; y < NET_SIZE_Y; y++)
		for(int x = 0; x < NET_SIZE_X; x++)
			if (net[y][x] == 1)
				paint_brick(x, y);
	
	for (i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++) {
		switch (seg->dir) {
		case HORIZONTAL:
			if (seg->y < 0)
				continue;

			for (int x = 0; x < seg->brick_cnt; x++)
				paint_brick(seg->x + x, seg->y);
			break;
		case VERTICAL:
			for (int y = 0; y < seg->brick_cnt; y++ )
				if (seg->y + y >= 0)
					paint_brick(seg->x, seg->y + y);
			break;
		}
	}
	wrefresh(game_erea);
}
void move_steps(struct object *obj, int step, int move_dir)
{
	int i;
	struct segment	*seg;
	
	switch (move_dir) {
	case CTRL_RIGHT:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++)
			seg->x += step;
		break;
	case CTRL_LEFT:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++)
			seg->x -= step;
		break;
	case CTRL_DOWN:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++)
			seg->y += step;
		break;
	case CTRL_UP:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++)
			seg->y -= step;
		break;
	}
}
int check_position_v(struct object *obj)
{
	int i, j;
	int flag;
	struct segment	*seg;

	/* check if the object reach the bottom */
	for (i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++)
		if (seg->dir == HORIZONTAL) {
			if (seg->y >= (NET_SIZE_Y - 1))
				return POSITION_STOP;
		} else
			if ((seg->y + seg->brick_cnt - 1) >= (NET_SIZE_Y - 1))
				return POSITION_STOP;
	
	/* check if the object touch other bricks */
	for (i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++) {
		flag = 0;
		switch (seg->dir) {
		case HORIZONTAL:
			for (j = 0; j < seg->brick_cnt; j++)
				flag |= net[seg->y + 1][seg->x + j];
			break;
		case VERTICAL:
			flag |= net[seg->y + seg->brick_cnt][seg->x];
			break;
		}
		
		if (flag == 1) {
			if (obj->seg_head->y >= 0)
				return POSITION_STOP; 
			else
				return POSITION_TERMINATED;
		}
	}
	return POSITION_FREE;
}
void  clear_full_lines(char *clear_vec)
{
	int i;
	char (*new_net)[NET_SIZE_X] = (char(*)[NET_SIZE_X])malloc(NET_SIZE_Y * NET_SIZE_X);
	
	memset(new_net, 0, NET_SIZE_Y * NET_SIZE_X);
	
	int j = NET_SIZE_Y - 1;
	for (i = j; i >= cut_of_point; i--) {
		if (clear_vec[i] == 0)
			memcpy(new_net[j--], net[i], NET_SIZE_X);
	}
	cut_of_point += j - i;
	free(net);
	net = new_net;
}
/*
void debug_display_net()
{
	for (int i = 0; i < NET_SIZE_Y; i++)
		for (int j = 0; j < NET_SIZE_X; j++) {
			wmove(stdscr, i, j);
			if (net[i][j] == 1)
				waddch(stdscr, '*');
			else
				waddch(stdscr, ' ');
		}
	wrefresh(stdscr);
			
}
*/
void merge_cur_to_net(struct object *obj)
{
	int i, j, k;
	
	struct segment	*seg;
	int need_clear;
	
	char clear_vec[NET_SIZE_Y];
	memset(clear_vec, 0, NET_SIZE_Y);
	
	for(k = 0, seg = obj->seg_head; k < obj->seg_cnt; k++, seg++) {
		switch (seg->dir) {
		case HORIZONTAL:
			for (i = 0; i < seg->brick_cnt; i++)
				net[seg->y][seg->x + i] = 1;
			
			clear_vec[seg->y] = 1;
			for (i = 0; i < NET_SIZE_X; i++) {
				if (net[seg->y][i] == 0) {
					clear_vec[seg->y]  = 0;
					break;
				}
			}
			break;
		case VERTICAL:
			for (i = 0; i < seg->brick_cnt; i++)
				net[seg->y + i][seg->x] = 1;

			for (i = 0; i < seg->brick_cnt; i++) {
				clear_vec[seg->y + i] = 1;
				for (j = 0; j < NET_SIZE_X; j++)
					if (net[seg->y + i][j] == 0) {
						clear_vec[seg->y + i]  = 0;
						break;
					}
			}
			break;
		}
		if (cut_of_point > seg->y)
			cut_of_point = seg->y;
	}
	for (i = obj->seg_head->y, need_clear = 0; i < NET_SIZE_Y; i++)
		need_clear |= clear_vec[i];
	if (need_clear != 0)
		clear_full_lines(clear_vec);	
}
void new_current_obj(void)	
{
	int i = rand() % object_nr;

	delay	= DELAY_NORMAL;
	current = &obj_vec[i];
	current->init(current);
}
int check_position_h(struct object * obj, int	move_dir)
{
	int		 i, j;
	struct segment	*seg;
	
	switch(move_dir) {
	case CTRL_LEFT:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++) {
			if (seg->x <= 0 )
				return POSITION_STOP;
			
			if (seg->dir == HORIZONTAL) {
				if (seg->y >= 0)
					if (net[seg->y][seg->x - 1]		== 1)
						return POSITION_STOP;
			} else {
				for (j = 0; j < seg->brick_cnt; j ++)
					if ((seg->y + j) >= 0 )
						if (net[seg->y + j][seg->x - 1] == 1)
							return POSITION_STOP;
			}
		}
		break;
	case CTRL_RIGHT:
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++) {
			if (seg->dir == HORIZONTAL) {
				if ((seg->x + seg->brick_cnt -1) >= (NET_SIZE_X - 1))	/* check to wall */
					return POSITION_STOP;
				
				if (seg->y >= 0)
					if (net[seg->y][seg->x + seg->brick_cnt] == 1)	/* check to other brick */
						return POSITION_STOP;
			} else {
				if (seg->x >= NET_SIZE_X -1)
					return POSITION_STOP;
				
				for (j = 0; j < seg->brick_cnt; j ++)
					if ((seg->y + j) >= 0 )
						if (net[seg->y + j][seg->x + 1] == 1)
							return POSITION_STOP;
			}
		}
		break;
	}
	return POSITION_FREE;
}
void parallel_move(struct object *obj, int move_dir)
{
	if (check_position_h(current ,move_dir) == POSITION_STOP)
		return;

	move_steps(current, 1, move_dir);
}
int get_ctrl_key()
{
	int	key;
	
	keypad(game_erea, TRUE);
	
	switch (wgetch(game_erea)) {
	case KEY_LEFT:
		key = CTRL_LEFT;
		break;
	case KEY_RIGHT:
		key = CTRL_RIGHT;
		break;
	case KEY_DOWN:
		key = CTRL_DOWN;
		break;
	case ' ':		/* Backspace */
		key = CTRL_BACKSPACE;
		break;
	case 10:		/* Enter Key */
		key = CTRL_ENTER;
		break;
	default:
		key = CTRL_OTHER;
		break;
	}
	return key;
}
void change_speed()
{
	delay = DELAY_FAST;
//	pthread_kill(main_tid, SIGUSR1);	
}
void sig_wakeup(int signo) {}

void resume_and_pause()
{
	if (pause_flag == 1){
		pause_flag = 0;
		pthread_kill(main_tid, SIGUSR1);
	} else 
		pause_flag = 1;
}
void *ctrl_object(void	*data)
{
	int key;
	while(1) {
		key = get_ctrl_key();
		/* when main thread pause (pause_flag == 1), this thread doesn't respond input events except "Enter" */
		if (key != CTRL_ENTER && pause_flag  == 1)
			continue;
		
		pthread_mutex_lock(&current->lock);
		
		struct object	*obj = current;
		switch (key) {
		case CTRL_LEFT:
		case CTRL_RIGHT:
			parallel_move(obj, key);
			display_game_erea();
			break;
		case CTRL_BACKSPACE:
			com_transfer(obj);
			display_game_erea();
			break;
		case CTRL_DOWN:
			change_speed();
			break;
		case CTRL_ENTER:
			resume_and_pause();
			break;
		}
		
		pthread_mutex_unlock(&current->lock);
	}
}
void game_over()
{
	endwin();
	exit(0);
}
int main()
{
	pthread_t tid;
	int obj_position;

	main_tid = pthread_self();
	/**
	 * posix recommends sigaction() ,deprecates signal(), but sigaction() isn't part of standard c,
	 * so we can't compile the code with option -std=c99 when using sigaction() in it.we can compile it with option -std=gnu99 instead
	*/
	own_signal(SIGUSR1, sig_wakeup); /* sigaction implement */
	
	net = (char(*)[NET_SIZE_X])malloc((NET_SIZE_Y) * (NET_SIZE_X));
	memset(net, 0, NET_SIZE_Y * NET_SIZE_X);
	
	srand((unsigned int)time(NULL));
	initscr();
	game_erea = derwin(stdscr, WINDOW_Y, WINDOW_X, 0, (COLS - WINDOW_X) / 2);

	noecho();
	curs_set(0);

	new_current_obj();
	pthread_create(&tid, NULL, ctrl_object, NULL);	
	
	while(1) {
		pthread_mutex_lock(&current->lock);
		
		struct object *obj = current;
		struct segment *seg;
		int i;
		int check_flag = 0;
		
		for(i = 0, seg = obj->seg_head; i < obj->seg_cnt; i++, seg++) {
			/**
			 *   we don't check the object's position until the lowest brick reach the cut_of_point */
			if (((seg->dir == HORIZONTAL) && (seg->y >= (cut_of_point -1))) ||
			    (((seg->y + seg->brick_cnt -1) >= (cut_of_point -1)) && (seg->dir == VERTICAL))) {
				check_flag = 1;
				break;
			}
		}
		if (check_flag == 1)
		{
			obj_position = check_position_v(obj);
			
			if (obj_position == POSITION_STOP) {	
				merge_cur_to_net(obj);
					
				pthread_mutex_unlock(&obj->lock);
				new_current_obj();
				continue;
			} else if (obj_position == POSITION_TERMINATED)
				game_over();
		}
		
		move_steps(obj, 1, CTRL_DOWN);
		display_game_erea();
		
		pthread_mutex_unlock(&current->lock);
		usleep(delay);
		/*check if this thread is paused by the other thread */
		if (pause_flag == 1)
			pause();
	}	
	return 0;
}
