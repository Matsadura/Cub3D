#include "game.h"

#include <string.h>

t_map_error	game_load_level(t_game *g, int index)
{
	t_map		next;
	t_map_error	err;

	err = map_load(g->levels[index], &next);
	if (err != MAP_OK)
	{
		map_free(&g->map);
		g->map = next;
		g->error = err;
		g->error_level = index;
		g->state = GAME_FATAL;
		return (err);
	}
	map_free(&g->map);
	g->map = next;
	g->level_index = index;
	g->level_generation++;
	g->level_time = 0.0;
	g->state = GAME_PLAYING;
	player_spawn(&g->player, &g->map);
	return (MAP_OK);
}

t_map_error	game_init(t_game *g, const char *const *levels, int count)
{
	memset(g, 0, sizeof(*g));
	g->levels = levels;
	g->level_count = count;
	g->show_minimap = true;
	if (count <= 0)
		return (MAP_ERR_NO_MAP);
	return (game_load_level(g, 0));
}

void	game_update(t_game *g, const t_input *in, double dt)
{
	if (g->state == GAME_FATAL)
		return ;
	if (input_pressed(in, ACT_MAP))
		g->show_minimap = !g->show_minimap;
	if (input_pressed(in, ACT_PAUSE))
		g->state = (g->state == GAME_PAUSED) ? GAME_PLAYING : GAME_PAUSED;
	if (g->state == GAME_PAUSED && input_pressed(in, ACT_CONFIRM))
		g->state = GAME_PLAYING;
	if (g->state != GAME_PLAYING)
		return ;
	if (input_pressed(in, ACT_RESTART))
	{
		player_spawn(&g->player, &g->map);
		g->level_time = 0.0;
		return ;
	}
	g->level_time += dt;
	player_update(&g->player, &g->map, in, dt);
}

void	game_free(t_game *g)
{
	map_free(&g->map);
}
