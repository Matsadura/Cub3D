#include "game.h"

#include <math.h>

#define GROWL_RANGE		7.0

static const int	g_dx[4] = {0, 1, 0, -1};
static const int	g_dy[4] = {-1, 0, 1, 0};

void	stalkers_spawn(t_game *g)
{
	t_stalker	*s;
	int			i;

	g->stalker_count = g->map.stalker_count;
	i = -1;
	while (++i < g->stalker_count)
	{
		s = &g->stalkers[i];
		*s = (t_stalker){0};
		s->x = g->map.stalkers[i].x + 0.5;
		s->y = g->map.stalkers[i].y + 0.5;
		s->to_x = g->map.stalkers[i].x;
		s->to_y = g->map.stalkers[i].y;
		s->from_x = s->to_x;
		s->from_y = s->to_y;
		s->mode = STALK_DORMANT;
		s->timer = STALKER_GRACE;
		s->growl_timer = 3.0 + (game_rand(g) % 400) / 100.0;
	}
}

static int	field(const t_game *g, int x, int y)
{
	if (map_is_solid(&g->map, x, y))
		return (-1);
	return (g->dist[y * g->map.w + x]);
}

/* Hunting: step to the neighbour closest to the player along corridors.
** Wandering: a random open neighbour, never straight back unless stuck. */
static void	choose_next(t_game *g, t_stalker *s)
{
	int	cx;
	int	cy;
	int	d;
	int	best;
	int	open[4];
	int	n;

	cx = s->to_x;
	cy = s->to_y;
	best = -1;
	n = 0;
	d = -1;
	while (++d < 4)
	{
		if (field(g, cx + g_dx[d], cy + g_dy[d]) >= 0 && (best < 0
				|| field(g, cx + g_dx[d], cy + g_dy[d])
				< field(g, cx + g_dx[best], cy + g_dy[best])))
			best = d;
		if (!map_is_solid(&g->map, cx + g_dx[d], cy + g_dy[d])
			&& (cx + g_dx[d] != s->from_x || cy + g_dy[d] != s->from_y))
			open[n++] = d;
	}
	if (s->mode != STALK_HUNT && s->mode != STALK_SEARCH)
		best = -1;
	if (best < 0 && n > 0)
		best = open[game_rand(g) % n];
	if (best < 0 && !map_is_solid(&g->map, s->from_x, s->from_y))
	{
		s->to_x = s->from_x;
		s->to_y = s->from_y;
		s->from_x = cx;
		s->from_y = cy;
		return ;
	}
	s->from_x = cx;
	s->from_y = cy;
	if (best < 0)
		return ;
	s->to_x = cx + g_dx[best];
	s->to_y = cy + g_dy[best];
}

static bool	perceives(const t_game *g, const t_stalker *s)
{
	double	dx;
	double	dy;
	double	range;
	int		heard;

	dx = g->player.x - s->x;
	dy = g->player.y - s->y;
	range = SIGHT_DARK + (SIGHT_LIT - SIGHT_DARK) * (flashlight_level(g) > 0);
	if (dx * dx + dy * dy <= range * range
		&& line_of_sight(&g->map, s->x, s->y, g->player.x, g->player.y))
		return (true);
	heard = field(g, (int)s->x, (int)s->y);
	return (heard >= 0 && heard <= g->noise);
}

static void	think(t_game *g, t_stalker *s, double dt)
{
	if (perceives(g, s))
	{
		if (s->mode != STALK_HUNT)
		{
			g->events |= EV_SPOTTED;
			game_message(g, MSG_SPOTTED);
		}
		s->mode = STALK_HUNT;
		s->timer = 0.0;
	}
	else if (s->mode == STALK_HUNT && (s->timer += dt) > STALKER_LOSE_TIME)
	{
		s->mode = STALK_SEARCH;
		s->timer = STALKER_SEARCH_TIME;
	}
	else if (s->mode == STALK_SEARCH && (s->timer -= dt) <= 0.0)
		s->mode = STALK_WANDER;
}

/* Walks toward cell centres, carrying leftover distance into the next
** cell so the path is the same at any frame rate. In the player's cell a
** hunting stalker goes straight for them. */
static void	walk(t_game *g, t_stalker *s, double step)
{
	double	tx;
	double	ty;
	double	len;
	bool	direct;
	int		guard;

	guard = 0;
	while (step > 1e-9 && guard++ < 8)
	{
		direct = s->mode == STALK_HUNT
			&& (int)floor(s->x) == (int)floor(g->player.x)
			&& (int)floor(s->y) == (int)floor(g->player.y);
		tx = direct ? g->player.x : s->to_x + 0.5;
		ty = direct ? g->player.y : s->to_y + 0.5;
		len = hypot(tx - s->x, ty - s->y);
		if (len > step)
		{
			s->x += (tx - s->x) / len * step;
			s->y += (ty - s->y) / len * step;
			return ;
		}
		s->x = tx;
		s->y = ty;
		step -= len;
		if (direct)
			return ;
		tx = s->to_x;
		ty = s->to_y;
		choose_next(g, s);
		if (s->to_x == (int)tx && s->to_y == (int)ty)
			return ;
	}
}

static double	speed(const t_game *g, const t_stalker *s)
{
	if (s->mode == STALK_HUNT)
		return (STALKER_HUNT + STALKER_HUNT_STEP * g->level_index);
	if (s->mode == STALK_SEARCH)
		return (STALKER_SEARCH);
	if (s->mode == STALK_WANDER)
		return (STALKER_WANDER);
	return (0.0);
}

static void	growl(t_game *g, t_stalker *s, double dt)
{
	double	dx;
	double	dy;

	s->growl_timer -= dt;
	if (s->growl_timer > 0.0)
		return ;
	s->growl_timer = 4.0 + (game_rand(g) % 500) / 100.0;
	dx = g->player.x - s->x;
	dy = g->player.y - s->y;
	if (dx * dx + dy * dy < GROWL_RANGE * GROWL_RANGE)
		g->events |= EV_GROWL;
}

void	stalkers_update(t_game *g, double dt)
{
	t_stalker	*s;
	int			i;

	i = -1;
	while (++i < g->stalker_count && g->state == GAME_PLAYING)
	{
		s = &g->stalkers[i];
		s->anim += dt;
		if (s->mode == STALK_DORMANT && (s->timer -= dt) <= 0.0)
			s->mode = STALK_WANDER;
		if (s->mode != STALK_DORMANT)
		{
			think(g, s, dt);
			walk(g, s, speed(g, s) * dt);
			growl(g, s, dt);
		}
		if (fabs(s->x - g->player.x) < CATCH_RADIUS
			&& fabs(s->y - g->player.y) < CATCH_RADIUS)
		{
			g->state = GAME_CAUGHT;
			g->state_timer = 0.0;
			g->catcher = i;
			g->events |= EV_CAUGHT;
		}
	}
}
