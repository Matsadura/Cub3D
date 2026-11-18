#include "render.h"

#include <stdio.h>
#include <string.h>

bool	renderer_init(t_renderer *r)
{
	memset(r, 0, sizeof(*r));
	r->generation = -1;
	return (true);
}

static void	free_textures(t_renderer *r)
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
	free_textures(r);
	i = 0;
	while (i < SIDE_COUNT)
	{
		if (!platform_load_image(g->map.tex[i], &r->tex[i]))
		{
			snprintf(r->error, sizeof(r->error), "cannot load texture %s: %s",
				g->map.tex[i], platform_error());
			free_textures(r);
			return (false);
		}
		i++;
	}
	r->generation = g->level_generation;
	return (true);
}

void	renderer_draw(t_renderer *r, t_platform *p, const t_game *g)
{
	uint32_t	*fb;

	fb = platform_framebuffer(p);
	draw_world(r, fb, g);
	if (g->show_minimap)
		draw_minimap(r, fb, g);
	platform_clear(p, 0x000000FFu);
	platform_blit_framebuffer(p);
	draw_overlays(p, g);
}

void	renderer_free(t_renderer *r)
{
	free_textures(r);
	r->generation = -1;
}
