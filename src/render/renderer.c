#include "render.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOB_PIXELS	3.0

static const char	*g_sprite_paths[SPR_COUNT] = {
	"assets/sprites/key.png",
	"assets/sprites/battery.png",
	"assets/sprites/stalker_0.png",
	"assets/sprites/stalker_1.png",
	"assets/sprites/face.png",
	"assets/sprites/hand.png",
};

static bool	load(t_renderer *r, t_image *img, const char *path)
{
	if (platform_load_image(path, img))
		return (true);
	snprintf(r->error, sizeof(r->error), "cannot load %s: %s", path,
		platform_error());
	return (false);
}

/* Sprites and the door are shared by every level: loaded once. */
bool	renderer_init(t_renderer *r)
{
	int	i;

	memset(r, 0, sizeof(*r));
	r->generation = -1;
	if (!light_init(r))
		return (snprintf(r->error, sizeof(r->error), "out of memory"), false);
	if (!load(r, &r->door, "assets/textures/door.png"))
		return (false);
	i = -1;
	while (++i < SPR_COUNT)
		if (!load(r, &r->spr[i], g_sprite_paths[i]))
			return (false);
	return (true);
}

static void	free_walls(t_renderer *r)
{
	int	i;

	i = 0;
	while (i < SIDE_COUNT)
		platform_free_image(&r->tex[i++]);
}

bool	renderer_sync(t_renderer *r, const t_game *g)
{
	int	i;

	if (r->generation == g->level_generation)
		return (true);
	free_walls(r);
	i = -1;
	while (++i < SIDE_COUNT)
	{
		if (!load(r, &r->tex[i], g->map.tex[i]))
		{
			snprintf(r->error, sizeof(r->error), "cannot load texture %s: %s",
				g->map.tex[i], platform_error());
			free_walls(r);
			return (false);
		}
	}
	r->generation = g->level_generation;
	return (true);
}

static void	render_frame(t_renderer *r, uint32_t *fb, const t_game *g)
{
	bool	in_play;

	in_play = (g->state == GAME_PLAYING || g->state == GAME_PAUSED);
	r->horizon = VIEW_H / 2 + (int)lround(sin(g->walk_dist * PI_D / 0.85
				* 2.0) * BOB_PIXELS * (in_play ? 1.0 : 0.0));
	r->beam = flashlight_level(g);
	if (g->state == GAME_TITLE)
		r->beam = 0.6;
	draw_world(r, fb, g);
	draw_sprites(r, fb, g);
	if (in_play)
		draw_hand(r, fb, g);
	if (in_play && g->show_minimap)
		draw_minimap(fb, g);
	draw_post(r, fb, g);
	if (g->state == GAME_CAUGHT)
		draw_caught(r, fb, g);
}

void	renderer_draw(t_renderer *r, t_platform *p, const t_game *g)
{
	render_frame(r, platform_framebuffer(p), g);
	platform_clear(p, 0x000000FFu);
	platform_blit_framebuffer(p);
	draw_hud(p, g);
	draw_screens(p, g);
}

void	renderer_free(t_renderer *r)
{
	int	i;

	free_walls(r);
	platform_free_image(&r->door);
	i = 0;
	while (i < SPR_COUNT)
		platform_free_image(&r->spr[i++]);
	free(r->cone);
	free(r->vignette);
	r->cone = NULL;
	r->vignette = NULL;
	r->generation = -1;
}
