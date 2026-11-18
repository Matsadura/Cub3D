#include "map_internal.h"

#include <stdlib.h>
#include <string.h>

#define MAP_CHARS " 01NSEW"

static bool	is_walkable(char c)
{
	return (c != '1' && c != ' ' && c != '\0');
}

/* Finds the map block [first, last]; blank lines inside it are errors. */
static t_map_error	map_bounds(t_lines *l, int *first, int *last, t_map *m)
{
	int	i;

	while (*first < l->count && is_blank(l->line[*first]))
		(*first)++;
	if (*first >= l->count)
		return (MAP_ERR_NO_MAP);
	*last = l->count - 1;
	while (*last > *first && is_blank(l->line[*last]))
		(*last)--;
	i = *first;
	while (i <= *last)
	{
		if (is_blank(l->line[i]))
			return (map_fail(m, MAP_ERR_BLANK_LINE, i + 1, 0));
		i++;
	}
	return (MAP_OK);
}

static t_map_error	check_chars_and_size(t_lines *l, int first, int last,
		t_map *m)
{
	int		i;
	size_t	len;
	size_t	j;

	if (last - first + 1 > MAP_MAX_H)
		return (map_fail(m, MAP_ERR_TOO_LARGE, first + MAP_MAX_H + 1, 0));
	m->h = last - first + 1;
	m->w = 0;
	i = first;
	while (i <= last)
	{
		len = strlen(l->line[i]);
		if (len > MAP_MAX_W)
			return (map_fail(m, MAP_ERR_TOO_LARGE, i + 1, MAP_MAX_W + 1));
		j = 0;
		while (j < len)
		{
			if (strchr(MAP_CHARS, l->line[i][j]) == NULL)
			{
				m->err_char = l->line[i][j];
				return (map_fail(m, MAP_ERR_BAD_CHAR, i + 1, (int)j + 1));
			}
			j++;
		}
		if ((int)len > m->w)
			m->w = (int)len;
		i++;
	}
	return (MAP_OK);
}

static t_map_error	fill_cells(t_lines *l, int first, t_map *m)
{
	int		y;
	int		x;
	int		players;
	char	c;

	m->cells = malloc((size_t)m->w * (size_t)m->h);
	if (m->cells == NULL)
		return (MAP_ERR_NOMEM);
	memset(m->cells, ' ', (size_t)m->w * (size_t)m->h);
	players = 0;
	y = -1;
	while (++y < m->h)
	{
		x = -1;
		while (l->line[first + y][++x] != '\0')
		{
			c = l->line[first + y][x];
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
			{
				if (++players > 1)
					return (map_fail(m, MAP_ERR_MULTI_PLAYER,
							first + y + 1, x + 1));
				m->spawn_x = x;
				m->spawn_y = y;
				m->spawn_dir = c;
				c = '0';
			}
			m->cells[y * m->w + x] = c;
		}
	}
	if (players == 0)
		return (MAP_ERR_NO_PLAYER);
	return (MAP_OK);
}

/*
** A walkable cell may not touch the void (spaces or the grid border) on any
** of its four sides: one linear pass, no recursion.
*/
static t_map_error	check_enclosed(t_map *m, int first)
{
	int	x;
	int	y;

	y = 0;
	while (y < m->h)
	{
		x = 0;
		while (x < m->w)
		{
			if (is_walkable(map_cell(m, x, y))
				&& (map_cell(m, x - 1, y) == ' '
					|| map_cell(m, x + 1, y) == ' '
					|| map_cell(m, x, y - 1) == ' '
					|| map_cell(m, x, y + 1) == ' '))
				return (map_fail(m, MAP_ERR_OPEN_WALL, first + y + 1, x + 1));
			x++;
		}
		y++;
	}
	return (MAP_OK);
}

t_map_error	map_parse_grid(t_lines *l, int first, t_map *m)
{
	int			last;
	t_map_error	err;

	err = map_bounds(l, &first, &last, m);
	if (err == MAP_OK)
		err = check_chars_and_size(l, first, last, m);
	if (err == MAP_OK)
		err = fill_cells(l, first, m);
	if (err == MAP_OK)
		err = check_enclosed(m, first);
	return (err);
}
