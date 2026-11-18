#ifndef RENDER_H
# define RENDER_H

# include "game.h"
# include "platform.h"

/*
** Presentation: a software raycaster writing into the platform's
** framebuffer, plus overlays drawn with platform rects (no SDL here).
*/

# define VIEW_W		640
# define VIEW_H		360
# define FOV_DEG	60.0

typedef struct s_hit
{
	double	dist;
	double	wall_x;
	int		side;
	t_side	face;
}	t_hit;

typedef struct s_renderer
{
	t_image		tex[SIDE_COUNT];
	int			generation;
	/* Perpendicular wall distance per screen column (sprites, minimap). */
	double		zbuf[VIEW_W];
	double		ray_dx[VIEW_W];
	double		ray_dy[VIEW_W];
	char		error[256];
}	t_renderer;

bool	renderer_init(t_renderer *r);
/* Loads the current level's textures when the level changed. */
bool	renderer_sync(t_renderer *r, const t_game *g);
void	renderer_draw(t_renderer *r, t_platform *p, const t_game *g);
void	renderer_free(t_renderer *r);

/* raycast.c */
void	cast_ray(const t_map *m, const t_player *pl, double rdx, double rdy,
			t_hit *hit);
void	draw_world(t_renderer *r, uint32_t *fb, const t_game *g);

/* minimap.c */
void	draw_minimap(const t_renderer *r, uint32_t *fb, const t_game *g);

/* font.c: 5x7 bitmap font drawn with rects (no SDL_ttf). */
int		text_width(const char *s, int scale);
void	draw_text(t_platform *p, const char *s, const int pos[2], int scale,
			uint32_t rgba);
void	draw_text_centered(t_platform *p, const char *s, const int pos[2],
			int scale, uint32_t rgba);

/* hud.c */
void	draw_overlays(t_platform *p, const t_game *g);

#endif
