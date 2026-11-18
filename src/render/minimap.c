#include "render.h"

#include <math.h>

/*
** Circular fog-of-war minimap centred on the player: only cells the player
** has seen are drawn, with the exit and any keys or batteries spotted.
** Stalkers never appear on it.
*/

#define MM_RADIUS	46
#define MM_CX		(10 + MM_RADIUS)
#define MM_CY		(10 + MM_RADIUS)
#define MM_SCALE	6.0
#define MM_WALL		0xFF6B6458u
#define MM_FLOOR	0xFF1E1C1Au
#define MM_DOOR		0xFF3CD25Au
#define MM_DARK		0xFF080808u
#define MM_RING		0xFF2A2622u
#define MM_KEY		0xFFF0C040u
#define MM_BATTERY	0xFF60E070u
#define MM_PLAYER	0xFFE8E0D0u

static void	put(uint32_t *fb, int x, int y, uint32_t c)
{
	if (x >= 0 && y >= 0 && x < VIEW_W && y < VIEW_H)
		fb[y * VIEW_W + x] = c;
}

static uint32_t	cell_color(const t_game *g, int x, int y)
{
	char	c;

	if (x < 0 || y < 0 || x >= g->map.w || y >= g->map.h
		|| !g->seen[y * g->map.w + x])
		return (MM_DARK);
	c = map_cell(&g->map, x, y);
	if (c == 'X')
		return (MM_DOOR);
	if (c == '1')
		return (MM_WALL);
	if (c == ' ')
		return (MM_DARK);
	return (MM_FLOOR);
}

static void	draw_cells(uint32_t *fb, const t_game *g)
{
	int	dx;
	int	dy;
	int	d2;

	dy = -MM_RADIUS - 3;
	while (++dy < MM_RADIUS + 3)
	{
		dx = -MM_RADIUS - 3;
		while (++dx < MM_RADIUS + 3)
		{
			d2 = dx * dx + dy * dy;
			if (d2 > (MM_RADIUS + 2) * (MM_RADIUS + 2))
				continue ;
			if (d2 > MM_RADIUS * MM_RADIUS)
				put(fb, MM_CX + dx, MM_CY + dy, MM_RING);
			else
				put(fb, MM_CX + dx, MM_CY + dy, cell_color(g,
						(int)floor(g->player.x + dx / MM_SCALE),
						(int)floor(g->player.y + dy / MM_SCALE)));
		}
	}
}

static void	dot(uint32_t *fb, double wx, double wy, const t_game *g,
		uint32_t c)
{
	int	x;
	int	y;
	int	i;

	x = MM_CX + (int)lround((wx - g->player.x) * MM_SCALE);
	y = MM_CY + (int)lround((wy - g->player.y) * MM_SCALE);
	if ((x - MM_CX) * (x - MM_CX) + (y - MM_CY) * (y - MM_CY)
		> (MM_RADIUS - 2) * (MM_RADIUS - 2))
		return ;
	i = -1;
	while (++i < 9)
		put(fb, x - 1 + i % 3, y - 1 + i / 3, c);
}

static void	draw_player(uint32_t *fb, const t_game *g)
{
	double	t;

	t = 0.0;
	while (t < 6.0)
	{
		put(fb, MM_CX + (int)lround(cos(g->player.angle) * t),
			MM_CY + (int)lround(sin(g->player.angle) * t), MM_PLAYER);
		t += 0.5;
	}
	put(fb, MM_CX, MM_CY, MM_PLAYER);
	put(fb, MM_CX + 1, MM_CY, MM_PLAYER);
	put(fb, MM_CX, MM_CY + 1, MM_PLAYER);
	put(fb, MM_CX + 1, MM_CY + 1, MM_PLAYER);
}

void	draw_minimap(uint32_t *fb, const t_game *g)
{
	const t_pickup	*p;
	int				i;

	draw_cells(fb, g);
	i = -1;
	while (++i < g->pickup_count)
	{
		p = &g->pickups[i];
		if (!p->taken && g->seen[(int)p->y * g->map.w + (int)p->x])
			dot(fb, p->x, p->y, g, p->kind == 'K' ? MM_KEY : MM_BATTERY);
	}
	draw_player(fb, g);
}
