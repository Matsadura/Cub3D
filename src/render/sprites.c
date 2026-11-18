#include "render.h"

#include <math.h>

#define MAX_SPRITES	(MAP_MAX_ITEMS + MAP_MAX_STALKERS)

typedef struct s_sprite
{
	double			x;
	double			y;
	double			depth;
	double			cam_x;
	/* Vertical offset in tiles (pickups bob) and size scale. */
	double			lift;
	double			scale;
	const t_image	*img;
}	t_sprite;

static int	collect(const t_renderer *r, const t_game *g, t_sprite *out)
{
	int	n;
	int	i;

	n = 0;
	i = -1;
	while (++i < g->pickup_count)
		if (!g->pickups[i].taken)
			out[n++] = (t_sprite){g->pickups[i].x, g->pickups[i].y, 0, 0,
				0.06 * sin(g->level_time * 3.0 + i), 0.5,
				&r->spr[g->pickups[i].kind == 'K' ? SPR_KEY : SPR_BATTERY]};
	i = -1;
	while (++i < g->stalker_count)
		if (g->state != GAME_CAUGHT || i != g->catcher)
			out[n++] = (t_sprite){g->stalkers[i].x, g->stalkers[i].y, 0, 0,
			0.0, 1.15, &r->spr[(g->stalkers[i].mode != STALK_DORMANT
				&& (int)(g->stalkers[i].anim * 3.0) % 2)
			? SPR_STALKER_1 : SPR_STALKER_0]};
	return (n);
}

/* Camera-space depth and screen x for each sprite, then far-to-near. */
static void	project_and_sort(const t_game *g, t_sprite *s, int n)
{
	double		dx;
	double		dy;
	double		plane;
	t_sprite	tmp;
	int			i;
	int			j;

	plane = tan(FOV_DEG * PI_D / 360.0);
	i = -1;
	while (++i < n)
	{
		dx = s[i].x - g->player.x;
		dy = s[i].y - g->player.y;
		s[i].depth = dx * cos(g->player.angle) + dy * sin(g->player.angle);
		s[i].cam_x = (-dx * sin(g->player.angle) + dy * cos(g->player.angle))
			/ plane;
	}
	i = 0;
	while (++i < n)
	{
		j = i;
		while (j > 0 && s[j - 1].depth < s[j].depth)
		{
			tmp = s[j];
			s[j] = s[j - 1];
			s[j - 1] = tmp;
			j--;
		}
	}
}

static void	draw_one(const t_renderer *r, uint32_t *fb, const t_sprite *s)
{
	double		p;
	int			size;
	int			cx;
	int			cy;
	int			x;
	int			y;
	uint32_t	px;

	p = (VIEW_W / 2.0) / tan(FOV_DEG * PI_D / 360.0);
	size = (int)(p / s->depth * s->scale);
	cx = (int)(VIEW_W / 2.0 * (1.0 + s->cam_x / s->depth));
	cy = r->horizon + (int)(p / s->depth * (0.5 - s->lift)) - size;
	x = cx - size / 2 - 1;
	while (++x < cx + size / 2 && size > 0)
	{
		if (x < 0 || x >= VIEW_W || r->zbuf[x] <= s->depth)
			continue ;
		y = (cy < 0) ? -1 - cy : -1;
		while (++y < size && cy + y < VIEW_H)
		{
			px = s->img->px[(y * s->img->h / size) * s->img->w
				+ (x - (cx - size / 2)) * s->img->w / size];
			if ((px >> 24) < 128)
				continue ;
			fb[(cy + y) * VIEW_W + x] = is_emissive(px) ? (px | 0xFF000000u)
				: shade(px, light_at(r, (cy + y) * VIEW_W + x, s->depth));
		}
	}
}

void	draw_sprites(t_renderer *r, uint32_t *fb, const t_game *g)
{
	t_sprite	list[MAX_SPRITES];
	int			n;
	int			i;

	n = collect(r, g, list);
	project_and_sort(g, list, n);
	i = -1;
	while (++i < n)
		if (list[i].depth > 0.15 && list[i].img->px != NULL)
			draw_one(r, fb, &list[i]);
}
