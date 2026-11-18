#include "render.h"

#include <math.h>

/* Map F/C colours are albedos; boosted so a full beam reads clearly. */
#define FLOOR_GAIN	2.0f
#define CEIL_GAIN	1.6f

typedef struct s_column
{
	int			x;
	double		dist;
	int			top;
	int			bottom;
	int			tx_col;
	double		line_h;
}	t_column;

static double	proj_dist(void)
{
	return ((VIEW_W / 2.0) / tan(FOV_DEG * PI_D / 360.0));
}

/* Texture column so that faces read left-to-right from the viewer's side. */
static int	texture_column(const t_hit *h, const t_image *t, double rdx,
		double rdy)
{
	int	tx;

	tx = (int)(h->wall_x * t->w);
	if (tx >= t->w)
		tx = t->w - 1;
	if ((h->side == 0 && rdx < 0) || (h->side == 1 && rdy > 0))
		tx = t->w - tx - 1;
	return (tx);
}

static void	draw_wall(t_renderer *r, uint32_t *fb, const t_column *c,
		const t_image *t)
{
	double		step;
	double		tex_pos;
	int			y;
	int			ty;
	uint32_t	px;

	step = t->h / c->line_h;
	y = c->top;
	tex_pos = (y - (r->horizon - c->line_h / 2.0)) * step;
	while (y < c->bottom)
	{
		ty = (int)tex_pos;
		if (ty < 0)
			ty = 0;
		else if (ty >= t->h)
			ty = t->h - 1;
		px = t->px[ty * t->w + c->tx_col];
		if (is_emissive(px))
			fb[y * VIEW_W + c->x] = px | 0xFF000000u;
		else
			fb[y * VIEW_W + c->x] = shade(px, light_at(r, y * VIEW_W + c->x,
						c->dist));
		tex_pos += step;
		y++;
	}
}

/* Cheap per-tile variation and seams so floor and ceiling show motion. */
static float	surface(double fx, double fy, bool ceiling)
{
	uint32_t	h;
	double		ux;
	double		uy;
	int			cx;
	int			cy;

	cx = (int)floor(fx * 2.0);
	cy = (int)floor(fy * 2.0);
	h = ((uint32_t)cx * 73856093u) ^ ((uint32_t)cy * 19349663u);
	h = (h ^ (h >> 13)) * 1274126177u;
	ux = fx * 2.0 - cx;
	uy = fy * 2.0 - cy;
	if (ceiling && (fx - floor(fx) < 0.06))
		return (0.55f);
	if (!ceiling && (ux < 0.04 || uy < 0.04))
		return (0.6f);
	return (0.82f + (float)(h % 23) / 100.0f);
}

static void	draw_flats(t_renderer *r, uint32_t *fb, const t_game *g,
		const t_column *c)
{
	double		d;
	int			y;
	uint32_t	color;
	bool		ceil;

	y = -1;
	while (++y < VIEW_H)
	{
		if (y >= c->top && y < c->bottom)
		{
			y = c->bottom - 1;
			continue ;
		}
		ceil = (y < r->horizon);
		d = proj_dist() / (2.0 * fabs(y - r->horizon + 0.5));
		color = 0xFF000000u | (ceil ? g->map.ceil_rgb : g->map.floor_rgb);
		color = shade(color, surface(g->player.x + d * r->ray_dx[c->x],
					g->player.y + d * r->ray_dy[c->x], ceil)
				* (ceil ? CEIL_GAIN : FLOOR_GAIN));
		fb[y * VIEW_W + c->x] = shade(color, light_at(r, y * VIEW_W + c->x,
					d));
	}
}

void	draw_world(t_renderer *r, uint32_t *fb, const t_game *g)
{
	double		plane;
	double		cam;
	t_hit		hit;
	t_column	c;
	const t_image	*t;

	plane = tan(FOV_DEG * PI_D / 360.0);
	c.x = -1;
	while (++c.x < VIEW_W)
	{
		cam = 2.0 * (c.x + 0.5) / VIEW_W - 1.0;
		r->ray_dx[c.x] = cos(g->player.angle) - sin(g->player.angle) * plane * cam;
		r->ray_dy[c.x] = sin(g->player.angle) + cos(g->player.angle) * plane * cam;
		cast_ray(&g->map, (const double [2]){g->player.x, g->player.y},
			(const double [2]){r->ray_dx[c.x], r->ray_dy[c.x]}, &hit);
		c.dist = fmax(hit.dist, 1e-4);
		r->zbuf[c.x] = c.dist;
		c.line_h = proj_dist() / c.dist;
		c.top = (int)fmax(0.0, r->horizon - c.line_h / 2.0);
		c.bottom = (int)fmin(VIEW_H, r->horizon + c.line_h / 2.0);
		t = (hit.cell == 'X') ? &r->door : &r->tex[hit.face];
		c.tx_col = texture_column(&hit, t, r->ray_dx[c.x], r->ray_dy[c.x]);
		draw_wall(r, fb, &c, t);
		draw_flats(r, fb, g, &c);
	}
}
