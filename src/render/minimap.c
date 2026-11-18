#include "render.h"

#include <math.h>

/*
** Circular minimap centred on the player (the bonus feature of cub3D):
** walls, floor, the field-of-view cone clipped at the walls, the player.
*/

#define MM_RADIUS	54
#define MM_CX		(10 + MM_RADIUS)
#define MM_CY		(10 + MM_RADIUS)
#define MM_SCALE	7.0
#define MM_BORDER	3
#define MM_WALL		0xFFDEB887u
#define MM_FLOOR	0xFF1C1C1Cu
#define MM_VOID		0xFF0A0A0Au
#define MM_RING		0xFF000000u
#define MM_RAY		0xFF3060FFu
#define MM_PLAYER	0xFF00FFFFu

static void	put(uint32_t *fb, int x, int y, uint32_t c)
{
	if (x >= 0 && y >= 0 && x < VIEW_W && y < VIEW_H)
		fb[y * VIEW_W + x] = c;
}

static void	draw_cells(uint32_t *fb, const t_game *g)
{
	int		dx;
	int		dy;
	int		d2;
	char	c;

	dy = -MM_RADIUS - MM_BORDER;
	while (++dy < MM_RADIUS + MM_BORDER)
	{
		dx = -MM_RADIUS - MM_BORDER;
		while (++dx < MM_RADIUS + MM_BORDER)
		{
			d2 = dx * dx + dy * dy;
			if (d2 > (MM_RADIUS + MM_BORDER) * (MM_RADIUS + MM_BORDER))
				continue ;
			if (d2 > MM_RADIUS * MM_RADIUS)
			{
				put(fb, MM_CX + dx, MM_CY + dy, MM_RING);
				continue ;
			}
			c = map_cell(&g->map, (int)floor(g->player.x + dx / MM_SCALE),
					(int)floor(g->player.y + dy / MM_SCALE));
			put(fb, MM_CX + dx, MM_CY + dy, (c == '1') ? MM_WALL
				: (c == ' ') ? MM_VOID : MM_FLOOR);
		}
	}
}

/* Every 8th view ray, drawn until it hits a wall or leaves the circle. */
static void	draw_fov(const t_renderer *r, uint32_t *fb)
{
	int		x;
	double	len;
	double	t;
	double	norm;

	x = 0;
	while (x < VIEW_W)
	{
		norm = sqrt(r->ray_dx[x] * r->ray_dx[x] + r->ray_dy[x] * r->ray_dy[x]);
		len = r->zbuf[x] * norm * MM_SCALE;
		if (len > MM_RADIUS - 1)
			len = MM_RADIUS - 1;
		t = 0.0;
		while (t < len)
		{
			put(fb, MM_CX + (int)lround(r->ray_dx[x] / norm * t),
				MM_CY + (int)lround(r->ray_dy[x] / norm * t), MM_RAY);
			t += 1.0;
		}
		x += 8;
	}
}

static void	draw_player(uint32_t *fb)
{
	int	dx;
	int	dy;

	dy = -3;
	while (++dy < 3)
	{
		dx = -3;
		while (++dx < 3)
			if (dx * dx + dy * dy <= 5)
				put(fb, MM_CX + dx, MM_CY + dy, MM_PLAYER);
	}
}

void	draw_minimap(const t_renderer *r, uint32_t *fb, const t_game *g)
{
	draw_cells(fb, g);
	draw_fov(r, fb);
	draw_player(fb);
}
