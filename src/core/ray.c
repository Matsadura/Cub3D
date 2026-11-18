#include "game.h"

#include <math.h>

/*
** Grid DDA: steps cell by cell along the ray until a solid cell (wall,
** door or void). Gives the perpendicular (fisheye-free) distance in units
** of |dir|, the hit side, the hit cell and where along the face it landed.
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

static void	dda_setup(t_dda *d, const double pos[2], const double dir[2])
{
	d->mx = (int)floor(pos[0]);
	d->my = (int)floor(pos[1]);
	d->delta_x = (dir[0] == 0.0) ? 1e30 : fabs(1.0 / dir[0]);
	d->delta_y = (dir[1] == 0.0) ? 1e30 : fabs(1.0 / dir[1]);
	d->step_x = (dir[0] < 0) ? -1 : 1;
	d->step_y = (dir[1] < 0) ? -1 : 1;
	if (dir[0] < 0)
		d->side_x = (pos[0] - d->mx) * d->delta_x;
	else
		d->side_x = (d->mx + 1.0 - pos[0]) * d->delta_x;
	if (dir[1] < 0)
		d->side_y = (pos[1] - d->my) * d->delta_y;
	else
		d->side_y = (d->my + 1.0 - pos[1]) * d->delta_y;
}

static void	finish_hit(const t_dda *d, const double pos[2],
		const double dir[2], t_hit *hit)
{
	if (hit->side == 0)
		hit->dist = d->side_x - d->delta_x;
	else
		hit->dist = d->side_y - d->delta_y;
	if (hit->side == 0)
		hit->wall_x = pos[1] + hit->dist * dir[1];
	else
		hit->wall_x = pos[0] + hit->dist * dir[0];
	hit->wall_x -= floor(hit->wall_x);
	if (hit->side == 0)
		hit->face = (dir[0] < 0) ? SIDE_WE : SIDE_EA;
	else
		hit->face = (dir[1] < 0) ? SIDE_NO : SIDE_SO;
}

void	cast_ray(const t_map *m, const double pos[2], const double dir[2],
		t_hit *hit)
{
	t_dda	d;
	int		guard;

	dda_setup(&d, pos, dir);
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
	hit->cell_x = d.mx;
	hit->cell_y = d.my;
	hit->cell = map_cell(m, d.mx, d.my);
	finish_hit(&d, pos, dir, hit);
}
