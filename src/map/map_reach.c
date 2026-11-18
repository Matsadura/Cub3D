#include "map_internal.h"

#include <stdlib.h>

/* Iterative 4-way flood fill over walkable cells from the player start. */
static bool	*flood(const t_map *m)
{
	bool	*seen;
	int		*queue;
	int		head;
	int		tail;
	int		c;
	int		d;
	int		x;
	int		y;
	static const int	dx[4] = {1, -1, 0, 0};
	static const int	dy[4] = {0, 0, 1, -1};

	seen = calloc((size_t)m->w * m->h, sizeof(bool));
	queue = malloc(sizeof(int) * (size_t)m->w * m->h);
	if (seen == NULL || queue == NULL)
		return (free(queue), free(seen), NULL);
	head = 0;
	tail = 0;
	queue[tail++] = m->spawn_y * m->w + m->spawn_x;
	seen[queue[0]] = true;
	while (head < tail)
	{
		c = queue[head++];
		d = -1;
		while (++d < 4)
		{
			x = c % m->w + dx[d];
			y = c / m->w + dy[d];
			if (!map_is_solid(m, x, y) && !seen[y * m->w + x])
			{
				seen[y * m->w + x] = true;
				queue[tail++] = y * m->w + x;
			}
		}
	}
	free(queue);
	return (seen);
}

static bool	reached(const t_map *m, const bool *seen, int x, int y)
{
	return (x >= 0 && y >= 0 && x < m->w && y < m->h && seen[y * m->w + x]);
}

static t_map_error	fail_at(t_map *m, t_map_error err, int x, int y)
{
	return (map_fail(m, err, m->grid_line + y + 1, x + 1));
}

/* At least one door must touch a reachable cell. */
static t_map_error	check_exits(t_map *m, const bool *seen)
{
	int	x;
	int	y;
	int	fx;
	int	fy;

	fx = -1;
	fy = -1;
	y = -1;
	while (++y < m->h)
	{
		x = -1;
		while (++x < m->w)
		{
			if (map_cell(m, x, y) != 'X')
				continue ;
			if (reached(m, seen, x - 1, y) || reached(m, seen, x + 1, y)
				|| reached(m, seen, x, y - 1) || reached(m, seen, x, y + 1))
				return (MAP_OK);
			if (fx < 0)
				fx = x;
			if (fy < 0)
				fy = y;
		}
	}
	return (fail_at(m, MAP_ERR_UNREACHABLE, fx, fy));
}

static t_map_error	check_spawns(t_map *m, const bool *seen)
{
	int		i;
	t_spawn	*s;
	int		dx;
	int		dy;

	i = -1;
	while (++i < m->item_count)
		if (!seen[m->items[i].y * m->w + m->items[i].x])
			return (fail_at(m, MAP_ERR_UNREACHABLE, m->items[i].x,
					m->items[i].y));
	i = -1;
	while (++i < m->stalker_count)
	{
		s = &m->stalkers[i];
		dx = s->x - m->spawn_x;
		dy = s->y - m->spawn_y;
		if (dx * dx + dy * dy < MAP_STALKER_MIN_DIST * MAP_STALKER_MIN_DIST)
			return (fail_at(m, MAP_ERR_STALKER_TOO_CLOSE, s->x, s->y));
	}
	return (MAP_OK);
}

t_map_error	map_check_reachable(t_map *m)
{
	bool		*seen;
	t_map_error	err;

	if (m->key_count > 0 && m->exit_count == 0)
		return (MAP_ERR_KEYS_NO_EXIT);
	if (m->item_count == 0 && m->stalker_count == 0 && m->exit_count == 0)
		return (MAP_OK);
	seen = flood(m);
	if (seen == NULL)
		return (MAP_ERR_NOMEM);
	err = check_spawns(m, seen);
	if (err == MAP_OK && m->exit_count > 0)
		err = check_exits(m, seen);
	free(seen);
	return (err);
}
