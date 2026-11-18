#include "game.h"

#include <stdlib.h>
#include <string.h>

/* xorshift32: deterministic per level, so scripted tests are exact. */
uint32_t	game_rand(t_game *g)
{
	g->rng ^= g->rng << 13;
	g->rng ^= g->rng >> 17;
	g->rng ^= g->rng << 5;
	return (g->rng);
}

static void	free_buffers(t_game *g)
{
	free(g->seen);
	free(g->dist);
	free(g->queue);
	g->seen = NULL;
	g->dist = NULL;
	g->queue = NULL;
}

static bool	alloc_buffers(t_game *g)
{
	size_t	n;

	n = (size_t)g->map.w * (size_t)g->map.h;
	g->seen = calloc(n, sizeof(bool));
	g->dist = malloc(n * sizeof(int));
	g->queue = malloc(n * sizeof(int));
	if (g->seen == NULL || g->dist == NULL || g->queue == NULL)
	{
		free_buffers(g);
		return (false);
	}
	return (true);
}

/* Puts the current level back to its start (after a death or restart). */
void	level_reset(t_game *g)
{
	int	i;

	player_spawn(&g->player, &g->map);
	g->rng = 0x9E3779B9u * (uint32_t)(g->level_index + 1);
	g->keys_held = 0;
	g->battery = 1.0;
	g->light_on = true;
	g->stamina = 1.0;
	g->exhausted = false;
	g->sprinting = false;
	g->noise = 0.0;
	g->walk_dist = 0.0;
	g->step_acc = 0.0;
	g->danger = 0.0;
	g->heartbeat = 0.0;
	g->level_time = 0.0;
	g->message = MSG_NONE;
	g->message_timer = 0.0;
	g->pickup_count = g->map.item_count;
	i = -1;
	while (++i < g->pickup_count)
		g->pickups[i] = (t_pickup){g->map.items[i].x + 0.5,
			g->map.items[i].y + 0.5, g->map.items[i].kind, false};
	memset(g->seen, 0, (size_t)g->map.w * (size_t)g->map.h);
	g->dist_cell = -1;
	sense_update_field(g);
	stalkers_spawn(g);
	sense_reveal(g);
}

t_map_error	level_load(t_game *g, int index)
{
	t_map		next;
	t_map_error	err;

	err = map_load(g->levels[index], &next);
	if (err == MAP_OK)
	{
		map_free(&g->map);
		free_buffers(g);
		g->map = next;
		if (!alloc_buffers(g))
			err = MAP_ERR_NOMEM;
	}
	else
	{
		map_free(&g->map);
		g->map = next;
	}
	if (err != MAP_OK)
	{
		g->error = err;
		g->error_level = index;
		g->state = GAME_FATAL;
		return (err);
	}
	g->level_index = index;
	g->level_generation++;
	g->free_roam = (g->map.exit_count == 0);
	level_reset(g);
	return (MAP_OK);
}
