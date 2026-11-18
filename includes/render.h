#ifndef RENDER_H
# define RENDER_H

# include "game.h"
# include "platform.h"

/*
** Presentation: a software raycaster with lighting writing into the
** platform's framebuffer, plus overlays drawn with platform rects.
** No SDL here.
*/

# define VIEW_W			640
# define VIEW_H			360
# define FOV_DEG		60.0
# define GAME_TITLE_TEXT	"DARK MAZE"

typedef enum e_sprite_img
{
	SPR_KEY,
	SPR_BATTERY,
	SPR_STALKER_0,
	SPR_STALKER_1,
	SPR_FACE,
	SPR_HAND,
	SPR_COUNT
}	t_sprite_img;

typedef struct s_renderer
{
	t_image		tex[SIDE_COUNT];
	t_image		door;
	t_image		spr[SPR_COUNT];
	int			generation;
	/* Static per-pixel tables: flashlight beam shape and vignette. */
	float		*cone;
	float		*vignette;
	/* Per frame: wall distance per column, view rays, horizon (bob). */
	double		zbuf[VIEW_W];
	double		ray_dx[VIEW_W];
	double		ray_dy[VIEW_W];
	int			horizon;
	double		beam;
	uint32_t	frame;
	char		error[256];
}	t_renderer;

bool		renderer_init(t_renderer *r);
/* Loads the current level's wall textures when the level changed. */
bool		renderer_sync(t_renderer *r, const t_game *g);
void		renderer_draw(t_renderer *r, t_platform *p, const t_game *g);
void		renderer_free(t_renderer *r);

/* light.c */
bool		light_init(t_renderer *r);
/* 0..~1.2 brightness of a point at distance d seen at screen pixel i. */
float		light_at(const t_renderer *r, int i, double d);
uint32_t	shade(uint32_t argb, float k);
bool		is_emissive(uint32_t argb);

/* raycast.c */
void		draw_world(t_renderer *r, uint32_t *fb, const t_game *g);

/* sprites.c */
void		draw_sprites(t_renderer *r, uint32_t *fb, const t_game *g);

/* effects.c */
void		blit_image(uint32_t *fb, const t_image *img, const int rect[4],
				float bright);
void		draw_hand(const t_renderer *r, uint32_t *fb, const t_game *g);
void		draw_post(t_renderer *r, uint32_t *fb, const t_game *g);
void		draw_caught(const t_renderer *r, uint32_t *fb, const t_game *g);

/* minimap.c */
void		draw_minimap(uint32_t *fb, const t_game *g);

/* font.c: 5x7 bitmap font drawn with rects (no SDL_ttf). */
int			text_width(const char *s, int scale);
void		draw_text(t_platform *p, const char *s, const int pos[2],
				int scale, uint32_t rgba);
void		draw_text_centered(t_platform *p, const char *s,
				const int pos[2], int scale, uint32_t rgba);

/* hud.c */
void		draw_hud(t_platform *p, const t_game *g);
void		level_title(const t_game *g, char *buf, size_t size);
void		format_time(double seconds, char *buf, size_t size);

/* screens.c */
void		draw_screens(t_platform *p, const t_game *g);

#endif
