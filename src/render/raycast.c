#include "render.h"

#include <math.h>

/*
** Grid DDA: steps cell by cell along the ray until a solid cell. Returns
** the perpendicular (fisheye-free) distance, the hit side and where along
** the wall face the ray landed.
*/
typedef struct s_dda
{
	int		mx;
	int		my;
	int		step_x;
	int		step_y;
	double	delta_x;
	double	delta_y;
	double	side_x;
	double	side_y;
}	t_dda;

static void	dda_setup(t_dda *d, const t_player *pl, double rdx, double rdy)
{
	d->mx = (int)floor(pl->x);
	d->my = (int)floor(pl->y);
	d->delta_x = (rdx == 0.0) ? 1e30 : fabs(1.0 / rdx);
	d->delta_y = (rdy == 0.0) ? 1e30 : fabs(1.0 / rdy);
	d->step_x = (rdx < 0) ? -1 : 1;
	d->step_y = (rdy < 0) ? -1 : 1;
	if (rdx < 0)
		d->side_x = (pl->x - d->mx) * d->delta_x;
	else
		d->side_x = (d->mx + 1.0 - pl->x) * d->delta_x;
	if (rdy < 0)
		d->side_y = (pl->y - d->my) * d->delta_y;
	else
		d->side_y = (d->my + 1.0 - pl->y) * d->delta_y;
}

void	cast_ray(const t_map *m, const t_player *pl, double rdx, double rdy,
		t_hit *hit)
{
	t_dda	d;
	int		guard;

	dda_setup(&d, pl, rdx, rdy);
	guard = 0;
	hit->side = 0;
	while (guard++ < (MAP_MAX_W + MAP_MAX_H) * 2)
	{
		hit->side = (d.side_x >= d.side_y);
		if (hit->side == 0)
			d.side_x += d.delta_x;
		else
			d.side_y += d.delta_y;
		d.mx += (hit->side == 0) * d.step_x;
		d.my += (hit->side == 1) * d.step_y;
		if (map_is_solid(m, d.mx, d.my))
			break ;
	}
	if (hit->side == 0)
		hit->dist = d.side_x - d.delta_x;
	else
		hit->dist = d.side_y - d.delta_y;
	if (hit->side == 0)
		hit->wall_x = pl->y + hit->dist * rdy;
	else
		hit->wall_x = pl->x + hit->dist * rdx;
	hit->wall_x -= floor(hit->wall_x);
	if (hit->side == 0)
		hit->face = (rdx < 0) ? SIDE_WE : SIDE_EA;
	else
		hit->face = (rdy < 0) ? SIDE_NO : SIDE_SO;
}

static void	fill_background(uint32_t *fb, const t_map *m)
{
	uint32_t	ceil_c;
	uint32_t	floor_c;
	int			i;

	ceil_c = 0xFF000000u | m->ceil_rgb;
	floor_c = 0xFF000000u | m->floor_rgb;
	i = 0;
	while (i < VIEW_W * VIEW_H / 2)
		fb[i++] = ceil_c;
	while (i < VIEW_W * VIEW_H)
		fb[i++] = floor_c;
}

/* Texture column so that faces read left-to-right from the viewer's side. */
static int	texture_column(const t_hit *h, const t_image *t, double rdx,
		double rdy)
{
	int	tx;

	tx = (int)(h->wall_x * t->w);
	if (tx >= t->w)
		tx = t->w - 1;
	if ((h->side == 0 && rdx > 0) || (h->side == 1 && rdy < 0))
		tx = t->w - tx - 1;
	return (tx);
}

static void	draw_column(uint32_t *fb, int x, const t_hit *h, const t_image *t,
		int tx)
{
	double	line_h;
	double	step;
	double	tex_pos;
	int		y;
	int		end;
	int		ty;

	line_h = (VIEW_W / 2.0) / tan(FOV_DEG * PI_D / 360.0) / h->dist;
	y = (int)(VIEW_H / 2.0 - line_h / 2.0);
	end = (int)(VIEW_H / 2.0 + line_h / 2.0);
	step = t->h / line_h;
	if (y < 0)
		y = 0;
	if (end > VIEW_H)
		end = VIEW_H;
	tex_pos = (y - VIEW_H / 2.0 + line_h / 2.0) * step;
	while (y < end)
	{
		ty = (int)tex_pos;
		if (ty < 0)
			ty = 0;
		else if (ty >= t->h)
			ty = t->h - 1;
		fb[y * VIEW_W + x] = t->px[ty * t->w + tx];
		tex_pos += step;
		y++;
	}
}

void	draw_world(t_renderer *r, uint32_t *fb, const t_game *g)
{
	double	plane;
	double	cam;
	t_hit	hit;
	int		x;

	fill_background(fb, &g->map);
	plane = tan(FOV_DEG * PI_D / 360.0);
	x = 0;
	while (x < VIEW_W)
	{
		cam = 2.0 * (x + 0.5) / VIEW_W - 1.0;
		r->ray_dx[x] = cos(g->player.angle) - sin(g->player.angle) * plane * cam;
		r->ray_dy[x] = sin(g->player.angle) + cos(g->player.angle) * plane * cam;
		cast_ray(&g->map, &g->player, r->ray_dx[x], r->ray_dy[x], &hit);
		if (hit.dist < 1e-4)
			hit.dist = 1e-4;
		r->zbuf[x] = hit.dist;
		draw_column(fb, x, &hit, &r->tex[hit.face],
			texture_column(&hit, &r->tex[hit.face], r->ray_dx[x],
				r->ray_dy[x]));
		x++;
	}
}
