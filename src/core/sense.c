#include "game.h"

#include <math.h>

#define REVEAL_RAYS		72
#define REVEAL_LIT		6.0
#define REVEAL_DARK		2.0

/*
** Breadth-first path distance from the player's cell to every reachable
** cell. Stalkers hunt by walking down this field, and hearing uses it
** (sound travels along corridors, not through walls). Only recomputed
** when the player enters a new cell.
*/
void	sense_update_field(t_game *g)
{
	static const int	dx[4] = {1, -1, 0, 0};
	static const int	dy[4] = {0, 0, 1, -1};
	int					head;
	int					tail;
	int					c;
	int					d;
	int					n;

	c = (int)g->player.y * g->map.w + (int)g->player.x;
	if (c == g->dist_cell)
		return ;
	g->dist_cell = c;
	n = -1;
	while (++n < g->map.w * g->map.h)
		g->dist[n] = -1;
	head = 0;
	tail = 0;
	g->queue[tail++] = c;
	g->dist[c] = 0;
	while (head < tail)
	{
		c = g->queue[head++];
		d = -1;
		while (++d < 4)
		{
			n = c + dy[d] * g->map.w + dx[d];
			if (!map_is_solid(&g->map, c % g->map.w + dx[d],
					c / g->map.w + dy[d]) && g->dist[n] < 0)
			{
				g->dist[n] = g->dist[c] + 1;
				g->queue[tail++] = n;
			}
		}
	}
}

/* Clear straight line between two points (no wall, door or void). */
bool	line_of_sight(const t_map *m, double x0, double y0, double x1,
		double y1)
{
	t_hit	hit;
	double	dx;
	double	dy;
	double	len;

	dx = x1 - x0;
	dy = y1 - y0;
	len = sqrt(dx * dx + dy * dy);
	if (len < 1e-6)
		return (true);
	cast_ray(m, (const double [2]){x0, y0}, (const double [2]){dx / len,
		dy / len}, &hit);
	return (hit.dist >= len);
}

static void	mark(t_game *g, int x, int y)
{
	if (x >= 0 && y >= 0 && x < g->map.w && y < g->map.h)
		g->seen[y * g->map.w + x] = true;
}

/* Fog of war: reveal what the player could see around them. */
void	sense_reveal(t_game *g)
{
	t_hit	hit;
	double	a;
	double	range;
	double	t;
	int		i;

	range = REVEAL_DARK + (REVEAL_LIT - REVEAL_DARK) * flashlight_level(g);
	i = -1;
	while (++i < REVEAL_RAYS)
	{
		a = 2.0 * PI_D * i / REVEAL_RAYS;
		cast_ray(&g->map, (const double [2]){g->player.x, g->player.y},
			(const double [2]){cos(a), sin(a)}, &hit);
		if (hit.dist <= range)
			mark(g, hit.cell_x, hit.cell_y);
		t = 0.0;
		while (t < hit.dist && t < range)
		{
			mark(g, (int)floor(g->player.x + cos(a) * t),
				(int)floor(g->player.y + sin(a) * t));
			t += 0.35;
		}
	}
}
