#include "render.h"

#include <math.h>

#define HAND_SCALE	2

/* Nearest-neighbour scaled copy; transparent pixels skipped, emissive
** pixels kept at full brightness, the rest multiplied by bright. */
static void	blit(uint32_t *fb, const t_image *img, const int rect[4],
		float bright, bool glow)
{
	int			x;
	int			y;
	uint32_t	px;

	if (img->px == NULL || rect[2] <= 0 || rect[3] <= 0)
		return ;
	y = -1;
	while (++y < rect[3])
	{
		if (rect[1] + y < 0 || rect[1] + y >= VIEW_H)
			continue ;
		x = -1;
		while (++x < rect[2])
		{
			if (rect[0] + x < 0 || rect[0] + x >= VIEW_W)
				continue ;
			px = img->px[(y * img->h / rect[3]) * img->w
				+ x * img->w / rect[2]];
			if ((px >> 24) < 128)
				continue ;
			fb[(rect[1] + y) * VIEW_W + rect[0] + x] = (glow
					&& is_emissive(px)) ? (px | 0xFF000000u) : shade(px, bright);
		}
	}
}

void	blit_image(uint32_t *fb, const t_image *img, const int rect[4],
		float bright)
{
	blit(fb, img, rect, bright, true);
}

/* The player's hand and flashlight, bobbing with each stride. */
void	draw_hand(const t_renderer *r, uint32_t *fb, const t_game *g)
{
	const t_image	*img;
	double			phase;
	int				bx;
	int				by;
	float			lit;

	img = &r->spr[SPR_HAND];
	phase = g->walk_dist * PI_D / 0.85;
	bx = (int)(sin(phase) * 5.0);
	by = (int)(fabs(cos(phase)) * 6.0) - (g->sprinting ? 0 : 3);
	lit = 0.35f + 0.55f * (float)r->beam;
	blit(fb, img, (const int [4]){VIEW_W - img->w * HAND_SCALE - 30 + bx,
		VIEW_H - img->h * HAND_SCALE + 18 + by, img->w * HAND_SCALE,
		img->h * HAND_SCALE}, lit, r->beam > 0.3);
}

/* Fast deterministic grain (xorshift per frame). */
static int	grain(uint32_t *seed, int amount)
{
	*seed ^= *seed << 13;
	*seed ^= *seed >> 17;
	*seed ^= *seed << 5;
	return ((int)(*seed % (uint32_t)(amount * 2 + 1)) - amount);
}

static uint32_t	post_pixel(uint32_t px, float dark, float red, int n)
{
	int	c[3];
	int	i;

	c[0] = (int)((px >> 16) & 0xFF);
	c[1] = (int)((px >> 8) & 0xFF);
	c[2] = (int)(px & 0xFF);
	i = -1;
	while (++i < 3)
	{
		c[i] = (int)(c[i] * (1.0f - dark)) + n;
		if (i == 0)
			c[i] += (int)(red * 90.0f);
		if (c[i] < 0)
			c[i] = 0;
		if (c[i] > 255)
			c[i] = 255;
	}
	return (0xFF000000u | (uint32_t)c[0] << 16 | (uint32_t)c[1] << 8
		| (uint32_t)c[2]);
}

/* Vignette tightening and reddening with danger, film grain, and a red
** flash when caught. */
void	draw_post(t_renderer *r, uint32_t *fb, const t_game *g)
{
	uint32_t	seed;
	float		v;
	float		danger;
	float		flash;
	int			i;

	seed = 0x2545F491u ^ (r->frame++ * 2654435761u);
	danger = (float)g->danger;
	flash = 0.0f;
	if (g->state == GAME_CAUGHT)
		danger = 0.0f;
	if (g->state == GAME_CAUGHT && g->state_timer < 0.35)
		flash = 1.0f - (float)g->state_timer / 0.35f;
	i = -1;
	while (++i < VIEW_W * VIEW_H)
	{
		v = r->vignette[i] * (0.55f + 0.4f * danger);
		fb[i] = post_pixel(fb[i], fminf(0.95f, v),
				fminf(1.0f, v * danger * 1.3f + flash),
				grain(&seed, 3 + (int)(danger * 7.0f)));
	}
}

/* Jump scare: the face rushes at the camera, shaking. */
void	draw_caught(const t_renderer *r, uint32_t *fb, const t_game *g)
{
	double	t;
	int		size;
	int		sx;
	int		sy;

	t = g->state_timer;
	if (t > 1.4)
		return ;
	size = (int)(VIEW_H * (0.4 + fmin(t, 0.35) / 0.35 * 1.1));
	sx = (int)(sin(t * 90.0) * 6.0);
	sy = (int)(cos(t * 77.0) * 5.0);
	blit_image(fb, &r->spr[SPR_FACE], (const int [4]){VIEW_W / 2 - size / 2
		+ sx, VIEW_H / 2 - size / 2 + sy + size / 10, size, size},
		0.8f + 0.2f * (float)sin(t * 40.0));
}
