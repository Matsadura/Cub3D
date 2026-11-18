#include "game.h"

#include <math.h>

#define DOOR_REACH		0.12
#define FLICKER_RATE	14.0

/* Flashlight toggle and sprint intent, before the player moves. */
void	survival_input(t_game *g, const t_input *in)
{
	if (input_pressed(in, ACT_LIGHT))
	{
		if (g->battery > 0.0)
			g->light_on = !g->light_on;
		g->events |= EV_LIGHT;
	}
	if (g->exhausted && g->stamina >= STAMINA_RESUME)
		g->exhausted = false;
	g->sprinting = input_held(in, ACT_SPRINT) && !g->exhausted
		&& g->stamina > 0.0;
}

/* Deterministic flicker below BATTERY_LOW: a hash of the time slot. */
double	flashlight_level(const t_game *g)
{
	uint32_t	h;
	double		level;

	if (!g->light_on || g->battery <= 0.0)
		return (0.0);
	level = 1.0;
	if (g->battery < BATTERY_LOW)
	{
		h = (uint32_t)(g->level_time * FLICKER_RATE) * 2654435761u;
		h ^= h >> 15;
		if (h % 100 < (uint32_t)(40.0 * (1.0 - g->battery / BATTERY_LOW)))
			level = 0.25;
		level *= 0.55 + 0.45 * g->battery / BATTERY_LOW;
	}
	return (level);
}

static void	drain(t_game *g, double dt)
{
	if (g->light_on && g->battery > 0.0)
	{
		g->battery -= BATTERY_DRAIN * dt;
		if (g->battery <= 0.0)
		{
			g->battery = 0.0;
			g->light_on = false;
			g->events |= EV_LIGHT_DEAD;
			game_message(g, MSG_LIGHT_DEAD);
		}
	}
	if (g->sprinting && g->noise > 0.0)
	{
		g->stamina -= STAMINA_DRAIN * dt;
		if (g->stamina <= 0.0)
		{
			g->stamina = 0.0;
			g->exhausted = true;
			g->events |= EV_EXHAUSTED;
			game_message(g, MSG_EXHAUSTED);
		}
	}
	else if (g->stamina < 1.0)
		g->stamina = fmin(1.0, g->stamina + STAMINA_REGEN * dt);
}

static void	collect(t_game *g)
{
	t_pickup	*p;
	int			i;

	i = -1;
	while (++i < g->pickup_count)
	{
		p = &g->pickups[i];
		if (p->taken || fabs(p->x - g->player.x) > PICKUP_RADIUS
			|| fabs(p->y - g->player.y) > PICKUP_RADIUS)
			continue ;
		p->taken = true;
		if (p->kind == 'K')
		{
			g->keys_held++;
			g->events |= EV_KEY;
			game_message(g, g->keys_held == g->map.key_count
				? MSG_ALL_KEYS : MSG_KEY);
		}
		else
		{
			g->battery = fmin(1.0, g->battery + BATTERY_PICKUP);
			g->light_on = true;
			g->events |= EV_BATTERY;
			game_message(g, MSG_BATTERY);
		}
	}
}

/* The player's box, grown by DOOR_REACH, overlaps an exit door. */
bool	player_touches_door(const t_game *g)
{
	double	r;
	int		x;
	int		y;

	r = PLAYER_RADIUS + DOOR_REACH;
	y = (int)floor(g->player.y - r);
	while (y <= (int)floor(g->player.y + r))
	{
		x = (int)floor(g->player.x - r);
		while (x <= (int)floor(g->player.x + r))
		{
			if (map_cell(&g->map, x, y) == 'X')
				return (true);
			x++;
		}
		y++;
	}
	return (false);
}

void	survival_update(t_game *g, double dt)
{
	drain(g, dt);
	collect(g);
	if (g->free_roam || !player_touches_door(g))
		return ;
	if (g->keys_held >= g->map.key_count)
	{
		g->state = GAME_ESCAPED;
		g->state_timer = 0.0;
		g->events |= EV_ESCAPE;
	}
	else if (g->message != MSG_LOCKED || g->message_timer <= 0.0)
	{
		g->events |= EV_LOCKED;
		game_message(g, MSG_LOCKED);
	}
}
