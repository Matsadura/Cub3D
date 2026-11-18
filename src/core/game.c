#include "game.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define HEARTBEAT_SLOW	1.3
#define HEARTBEAT_FAST	0.35

t_map_error	game_init(t_game *g, const char *const *levels, int count)
{
	t_map_error	err;

	memset(g, 0, sizeof(*g));
	g->levels = levels;
	g->level_count = count;
	g->show_minimap = true;
	g->lives = START_LIVES;
	if (count <= 0)
		return (MAP_ERR_NO_MAP);
	err = level_load(g, 0);
	if (err == MAP_OK)
		g->state = GAME_TITLE;
	return (err);
}

void	game_message(t_game *g, t_message msg)
{
	g->message = msg;
	g->message_timer = MESSAGE_TIME;
}

void	game_new_run(t_game *g)
{
	g->lives = START_LIVES;
	g->run_time = 0.0;
	g->events |= EV_START;
	if (g->level_index != 0 || g->map.cells == NULL)
	{
		if (level_load(g, 0) != MAP_OK)
			return ;
	}
	else
		level_reset(g);
	g->state = GAME_PLAYING;
}

/* 0..1 from the nearest stalker; drives the heartbeat and the vignette. */
static void	update_dread(t_game *g, double dt)
{
	double	d;
	double	best;
	int		i;

	best = 0.0;
	i = -1;
	while (++i < g->stalker_count)
	{
		d = hypot(g->stalkers[i].x - g->player.x,
				g->stalkers[i].y - g->player.y);
		d = 1.0 - d / DANGER_RANGE;
		if (g->stalkers[i].mode == STALK_HUNT)
			d += 0.25;
		if (d > best)
			best = d;
	}
	g->danger = fmin(1.0, best);
	if (g->danger < 0.05)
		return ;
	g->heartbeat -= dt;
	if (g->heartbeat <= 0.0)
	{
		g->heartbeat = HEARTBEAT_SLOW
			- (HEARTBEAT_SLOW - HEARTBEAT_FAST) * g->danger;
		g->events |= EV_HEARTBEAT;
	}
}

static void	play(t_game *g, const t_input *in, double dt)
{
	if (input_pressed(in, ACT_RESTART))
	{
		level_reset(g);
		return ;
	}
	g->level_time += dt;
	g->run_time += dt;
	if (g->message_timer > 0.0)
		g->message_timer -= dt;
	survival_input(g, in);
	player_update(g, in, dt);
	survival_update(g, dt);
	if (g->state != GAME_PLAYING)
		return ;
	sense_update_field(g);
	sense_reveal(g);
	stalkers_update(g, dt);
	update_dread(g, dt);
}

/* After a catch: lose a life, then retry the level or end the run. */
static void	after_caught(t_game *g, const t_input *in, double dt)
{
	g->state_timer += dt;
	if (g->state_timer < CAUGHT_TIME && !(g->state_timer > 0.6
			&& input_pressed(in, ACT_CONFIRM)))
		return ;
	g->lives--;
	if (g->lives <= 0)
	{
		g->state = GAME_OVER;
		g->state_timer = 0.0;
		g->events |= EV_GAME_OVER;
		return ;
	}
	level_reset(g);
	g->state = GAME_PLAYING;
}

static void	after_escape(t_game *g, const t_input *in)
{
	if (!input_pressed(in, ACT_CONFIRM))
		return ;
	if (g->level_index + 1 < g->level_count)
	{
		if (level_load(g, g->level_index + 1) == MAP_OK)
			g->state = GAME_PLAYING;
		return ;
	}
	g->state = GAME_VICTORY;
	g->state_timer = 0.0;
	g->events |= EV_VICTORY;
	if (g->best_time <= 0.0 || g->run_time < g->best_time)
		g->best_time = g->run_time;
}

void	game_update(t_game *g, const t_input *in, double dt)
{
	if (input_pressed(in, ACT_MAP))
		g->show_minimap = !g->show_minimap;
	if (g->state == GAME_PLAYING || g->state == GAME_PAUSED)
	{
		if (input_pressed(in, ACT_PAUSE) || (g->state == GAME_PAUSED
				&& input_pressed(in, ACT_CONFIRM)))
		{
			g->state = (g->state == GAME_PAUSED) ? GAME_PLAYING : GAME_PAUSED;
			g->events |= EV_PAUSE;
			return ;
		}
	}
	if (g->state == GAME_PLAYING)
		play(g, in, dt);
	else if (g->state == GAME_CAUGHT)
		after_caught(g, in, dt);
	else if (g->state == GAME_ESCAPED)
		after_escape(g, in);
	else if (g->state == GAME_TITLE || g->state == GAME_OVER)
	{
		if (input_pressed(in, ACT_CONFIRM))
			game_new_run(g);
	}
	else if (g->state == GAME_VICTORY && input_pressed(in, ACT_CONFIRM))
		g->state = GAME_TITLE;
	if (g->state != GAME_PLAYING)
		g->state_timer += (g->state != GAME_CAUGHT) * dt;
}

void	game_free(t_game *g)
{
	map_free(&g->map);
	free(g->seen);
	free(g->dist);
	free(g->queue);
	g->seen = NULL;
	g->dist = NULL;
	g->queue = NULL;
}
