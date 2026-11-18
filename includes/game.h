#ifndef GAME_H
# define GAME_H

# include "map.h"
# include "platform.h"

/*
** Pure game logic: no SDL, no drawing. Units are map cells (1.0 = one
** tile); every speed and timer is per second and scaled by the frame's dt.
**
** The horror-maze rules: find every key (K), reach the exit door (X)
** while stalkers (M) hunt you by sight and sound. The flashlight drains
** its battery (B refills it) and makes you visible from afar; sprinting
** uses stamina and makes noise. Three lives per run.
*/

# define PI_D				3.14159265358979323846

# define MOVE_SPEED			3.0
# define SPRINT_MULT		1.7
# define TURN_SPEED			2.2
# define MOUSE_SENS			0.0025
# define PLAYER_RADIUS		0.2
# define PICKUP_RADIUS		0.55
# define START_LIVES		3

# define BATTERY_DRAIN		(1.0 / 150.0)
# define BATTERY_PICKUP		0.5
# define BATTERY_LOW		0.2
# define STAMINA_DRAIN		(1.0 / 3.5)
# define STAMINA_REGEN		(1.0 / 6.0)
# define STAMINA_RESUME		0.3

/* Noise radii (path distance in cells) and sight ranges. */
# define NOISE_WALK			3.5
# define NOISE_SPRINT		9.0
# define SIGHT_LIT			12.0
# define SIGHT_DARK			3.0

# define STALKER_GRACE		3.0
# define STALKER_WANDER		1.1
# define STALKER_SEARCH		1.7
# define STALKER_HUNT		2.0
# define STALKER_HUNT_STEP	0.15
# define STALKER_LOSE_TIME	3.0
# define STALKER_SEARCH_TIME	6.0
# define CATCH_RADIUS		0.5

# define CAUGHT_TIME		2.2
# define MESSAGE_TIME		2.5
# define DANGER_RANGE		9.0

typedef enum e_state
{
	GAME_TITLE,
	GAME_PLAYING,
	GAME_PAUSED,
	GAME_CAUGHT,
	GAME_ESCAPED,
	GAME_OVER,
	GAME_VICTORY,
	GAME_FATAL,
	GAME_STATE_COUNT
}	t_state;

/* One-frame events for presentation (sound, effects). */
# define EV_STEP		(1u << 0)
# define EV_KEY			(1u << 1)
# define EV_BATTERY		(1u << 2)
# define EV_LOCKED		(1u << 3)
# define EV_ESCAPE		(1u << 4)
# define EV_SPOTTED		(1u << 5)
# define EV_CAUGHT		(1u << 6)
# define EV_HEARTBEAT	(1u << 7)
# define EV_LIGHT		(1u << 8)
# define EV_LIGHT_DEAD	(1u << 9)
# define EV_START		(1u << 10)
# define EV_PAUSE		(1u << 11)
# define EV_GAME_OVER	(1u << 12)
# define EV_VICTORY		(1u << 13)
# define EV_GROWL		(1u << 14)
# define EV_EXHAUSTED	(1u << 15)

typedef enum e_message
{
	MSG_NONE,
	MSG_LOCKED,
	MSG_KEY,
	MSG_ALL_KEYS,
	MSG_BATTERY,
	MSG_LIGHT_DEAD,
	MSG_SPOTTED,
	MSG_EXHAUSTED
}	t_message;

typedef enum e_stalk_mode
{
	STALK_DORMANT,
	STALK_WANDER,
	STALK_HUNT,
	STALK_SEARCH,
	STALK_MODE_COUNT
}	t_stalk_mode;

typedef struct s_player
{
	double	x;
	double	y;
	/* Radians, y axis pointing down: 0 = east, PI/2 = south. */
	double	angle;
}	t_player;

typedef struct s_stalker
{
	double			x;
	double			y;
	t_stalk_mode	mode;
	/* Cell it walks towards, and the cell it came from (wandering). */
	int				to_x;
	int				to_y;
	int				from_x;
	int				from_y;
	double			timer;
	double			growl_timer;
	double			anim;
}	t_stalker;

typedef struct s_pickup
{
	double	x;
	double	y;
	char	kind;
	bool	taken;
}	t_pickup;

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
	bool				free_roam;
	int					lives;
	int					keys_held;
	double				level_time;
	double				run_time;
	/* Best full-campaign time in seconds; 0 = none yet. */
	double				best_time;
	double				battery;
	bool				light_on;
	double				stamina;
	bool				exhausted;
	bool				sprinting;
	double				noise;
	double				walk_dist;
	double				step_acc;
	double				danger;
	double				heartbeat;
	double				state_timer;
	t_message			message;
	double				message_timer;
	t_pickup			pickups[MAP_MAX_ITEMS];
	int					pickup_count;
	t_stalker			stalkers[MAP_MAX_STALKERS];
	int					stalker_count;
	/* Index of the stalker that caught the player (for the jump scare). */
	int					catcher;
	/* Fog of war: cells the player has seen (w * h). */
	bool				*seen;
	/* Path distance from the player's cell (w * h; -1 = unreachable). */
	int					*dist;
	int					*queue;
	int					dist_cell;
	uint32_t			rng;
	uint32_t			events;
	t_map_error			error;
	int					error_level;
}	t_game;

/* game.c: state machine */
t_map_error	game_init(t_game *g, const char *const *levels, int count);
void		game_update(t_game *g, const t_input *in, double dt);
void		game_free(t_game *g);
void		game_new_run(t_game *g);
void		game_message(t_game *g, t_message msg);

/* level.c */
t_map_error	level_load(t_game *g, int index);
void		level_reset(t_game *g);
uint32_t	game_rand(t_game *g);

/* player.c */
void		player_spawn(t_player *pl, const t_map *m);
void		player_update(t_game *g, const t_input *in, double dt);
double		normalize_angle(double a);

/* survival.c: flashlight, stamina, pickups, the exit door */
void		survival_input(t_game *g, const t_input *in);
void		survival_update(t_game *g, double dt);
bool		player_touches_door(const t_game *g);
/* 0 (dark) .. 1 (full beam), including the low-battery flicker. */
double		flashlight_level(const t_game *g);

/* stalker.c */
void		stalkers_spawn(t_game *g);
void		stalkers_update(t_game *g, double dt);

/* sense.c: path field, line of sight, fog of war */
void		sense_update_field(t_game *g);
bool		line_of_sight(const t_map *m, double x0, double y0, double x1,
				double y1);
void		sense_reveal(t_game *g);

/* ray.c: grid DDA shared by the renderer and line-of-sight checks */
typedef struct s_hit
{
	double	dist;
	double	wall_x;
	int		side;
	int		cell_x;
	int		cell_y;
	char	cell;
	t_side	face;
}	t_hit;

void		cast_ray(const t_map *m, const double pos[2], const double dir[2],
				t_hit *hit);

#endif
