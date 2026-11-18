#include "game.h"

#include <math.h>

#define STEP_LENGTH		0.85

double	normalize_angle(double a)
{
	a = fmod(a, 2.0 * PI_D);
	if (a < 0)
		a += 2.0 * PI_D;
	return (a);
}

void	player_spawn(t_player *pl, const t_map *m)
{
	pl->x = m->spawn_x + 0.5;
	pl->y = m->spawn_y + 0.5;
	if (m->spawn_dir == 'E')
		pl->angle = 0.0;
	else if (m->spawn_dir == 'S')
		pl->angle = PI_D / 2.0;
	else if (m->spawn_dir == 'W')
		pl->angle = PI_D;
	else
		pl->angle = PI_D * 1.5;
}

/* True when the player's bounding box at (x, y) overlaps no solid cell. */
static bool	fits(const t_map *m, double x, double y)
{
	return (!map_is_solid(m, (int)floor(x - PLAYER_RADIUS),
			(int)floor(y - PLAYER_RADIUS))
		&& !map_is_solid(m, (int)floor(x + PLAYER_RADIUS),
			(int)floor(y - PLAYER_RADIUS))
		&& !map_is_solid(m, (int)floor(x - PLAYER_RADIUS),
			(int)floor(y + PLAYER_RADIUS))
		&& !map_is_solid(m, (int)floor(x + PLAYER_RADIUS),
			(int)floor(y + PLAYER_RADIUS)));
}

/* Largest fraction of d (by halving) that keeps the player clear of walls,
** so a blocked step still ends flush against the wall. */
static double	clear_step(const t_map *m, const t_player *pl, double d,
		bool along_x)
{
	int	i;

	i = 0;
	while (i < 6)
	{
		if (along_x && fits(m, pl->x + d, pl->y))
			return (d);
		if (!along_x && fits(m, pl->x, pl->y + d))
			return (d);
		d *= 0.5;
		i++;
	}
	return (0.0);
}

static void	turn(t_player *pl, const t_input *in, double dt)
{
	double	t;

	t = 0.0;
	if (input_held(in, ACT_TURN_L))
		t -= 1.0;
	if (input_held(in, ACT_TURN_R))
		t += 1.0;
	pl->angle = normalize_angle(pl->angle + t * TURN_SPEED * dt
			+ in->mouse_dx * MOUSE_SENS);
}

/* Footsteps: one EV_STEP per stride, louder (noise) when sprinting. */
static void	walked(t_game *g, double dist)
{
	g->walk_dist += dist;
	g->step_acc += dist;
	if (dist > 0.0)
		g->noise = g->sprinting ? NOISE_SPRINT : NOISE_WALK;
	if (g->step_acc >= STEP_LENGTH)
	{
		g->step_acc -= STEP_LENGTH;
		g->events |= EV_STEP;
	}
}

void	player_update(t_game *g, const t_input *in, double dt)
{
	t_player	*pl;
	double		fwd;
	double		side;
	double		len;
	double		ox;

	pl = &g->player;
	turn(pl, in, dt);
	g->noise = 0.0;
	fwd = (double)input_held(in, ACT_FORWARD) - input_held(in, ACT_BACK);
	side = (double)input_held(in, ACT_STRAFE_R) - input_held(in, ACT_STRAFE_L);
	len = sqrt(fwd * fwd + side * side);
	g->sprinting = g->sprinting && len > 0.0;
	if (len == 0.0)
		return ;
	len = MOVE_SPEED * (g->sprinting ? SPRINT_MULT : 1.0) * dt / len;
	fwd *= len;
	side *= len;
	ox = pl->x;
	len = pl->y;
	pl->x += clear_step(&g->map, pl,
			cos(pl->angle) * fwd - sin(pl->angle) * side, true);
	pl->y += clear_step(&g->map, pl,
			sin(pl->angle) * fwd + cos(pl->angle) * side, false);
	walked(g, sqrt((pl->x - ox) * (pl->x - ox) + (pl->y - len) * (pl->y
				- len)));
}
