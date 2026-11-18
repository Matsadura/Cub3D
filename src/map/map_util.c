#include "map_internal.h"

#include <string.h>

bool	is_blank(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (*s == '\0');
}

const char	*skip_ws(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (s);
}

/* True when s ends with suffix and has at least one char before it. */
bool	has_suffix(const char *s, const char *suffix)
{
	size_t	ls;
	size_t	lx;

	ls = strlen(s);
	lx = strlen(suffix);
	return (ls > lx && strcmp(s + ls - lx, suffix) == 0);
}

t_map_error	map_fail(t_map *m, t_map_error err, int line, int col)
{
	if (m->err_line == 0)
	{
		m->err_line = line;
		m->err_col = col;
	}
	return (err);
}

char	map_cell(const t_map *m, int x, int y)
{
	if (m->cells == NULL || x < 0 || y < 0 || x >= m->w || y >= m->h)
		return (' ');
	return (m->cells[y * m->w + x]);
}

bool	map_is_solid(const t_map *m, int x, int y)
{
	char	c;

	c = map_cell(m, x, y);
	return (c == '1' || c == ' ' || c == 'X');
}
