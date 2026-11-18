#ifndef GAME_H
# define GAME_H

# include "map.h"
# include "platform.h"

/*
** Pure game logic: no SDL, no drawing. Units are map cells (1.0 = one
** tile); every speed is per second and scaled by the frame's dt.
*/

# define MOVE_SPEED		3.0
# define TURN_SPEED		2.2
# define MOUSE_SENS		0.0025
# define PLAYER_RADIUS	0.2
# define PI_D			3.14159265358979323846

typedef enum e_state
{
	GAME_PLAYING,
	GAME_PAUSED,
	GAME_FATAL,
	GAME_STATE_COUNT
}	t_state;

typedef struct s_player
{
	double	x;
	double	y;
	/* Radians, y axis pointing down: 0 = east, PI/2 = south. */
	double	angle;
}	t_player;

typedef struct s_game
{
	t_state				state;
	t_map				map;
	t_player			player;
	const char *const	*levels;
	int					level_count;
	int					level_index;
	/* Bumped on every level load so the renderer reloads its textures. */
	int					level_generation;
	bool				show_minimap;
	double				level_time;
	/* One-frame presentation events (sound, effects); cleared each tick. */
	uint32_t			events;
	t_map_error			error;
	int					error_level;
}	t_game;

t_map_error	game_init(t_game *g, const char *const *levels, int count);
t_map_error	game_load_level(t_game *g, int index);
void		game_update(t_game *g, const t_input *in, double dt);
void		game_free(t_game *g);

void		player_spawn(t_player *pl, const t_map *m);
void		player_update(t_player *pl, const t_map *m, const t_input *in,
				double dt);
double		normalize_angle(double a);

#endif
