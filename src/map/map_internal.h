#ifndef MAP_INTERNAL_H
# define MAP_INTERNAL_H

# include "map.h"

/* The file split into lines, in place ('\n' and a trailing '\r' removed). */
typedef struct s_lines
{
	char	*buf;
	char	**line;
	int		count;
}	t_lines;

/* map_read.c */
t_map_error	map_read_lines(const char *path, t_lines *out);
void		lines_free(t_lines *l);

/* map_header.c: parses the six elements; *next = first line after them. */
t_map_error	map_parse_header(const char *map_path, t_lines *l, t_map *m,
				int *next);

/* map_grid.c: builds and validates the grid from lines[first..]. */
t_map_error	map_parse_grid(t_lines *l, int first, t_map *m);

/* map_reach.c: every key, battery, stalker and one exit reachable. */
t_map_error	map_check_reachable(t_map *m);

/* map_util.c */
bool		is_blank(const char *s);
const char	*skip_ws(const char *s);
bool		has_suffix(const char *s, const char *suffix);
t_map_error	map_fail(t_map *m, t_map_error err, int line, int col);

#endif
