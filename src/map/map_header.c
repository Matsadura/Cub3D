#include "map_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ELEM_F 4
#define ELEM_C 5
#define ELEM_COUNT 6

static const char	*g_ids[ELEM_COUNT] = {"NO", "SO", "WE", "EA", "F", "C"};

static bool	is_readable_file(const char *path)
{
	struct stat	st;

	return (stat(path, &st) == 0 && S_ISREG(st.st_mode)
		&& access(path, R_OK) == 0);
}

/*
** Texture paths are tried as given (relative to the working directory, as
** the original cub3D did), then relative to the .cub file's directory so
** that maps stay portable.
*/
static t_map_error	resolve_texture(const char *map_path, const char *tex,
		char **out)
{
	char		buf[MAP_MAX_PATH];
	const char	*slash;
	int			dir_len;

	slash = strrchr(tex, '/');
	if (slash == NULL)
		slash = tex;
	else
		slash++;
	if (!has_suffix(slash, ".png") && !has_suffix(slash, ".xpm"))
		return (MAP_ERR_TEXTURE_EXT);
	if (strlen(tex) >= MAP_MAX_PATH)
		return (MAP_ERR_TEXTURE_MISSING);
	if (!is_readable_file(tex))
	{
		slash = strrchr(map_path, '/');
		if (tex[0] == '/' || slash == NULL)
			return (MAP_ERR_TEXTURE_MISSING);
		dir_len = (int)(slash - map_path);
		if (snprintf(buf, sizeof(buf), "%.*s/%s", dir_len, map_path, tex)
			>= (int)sizeof(buf) || !is_readable_file(buf))
			return (MAP_ERR_TEXTURE_MISSING);
		tex = buf;
	}
	*out = strdup(tex);
	if (*out == NULL)
		return (MAP_ERR_NOMEM);
	return (MAP_OK);
}

/* Exactly one whitespace-free token after the identifier. */
static t_map_error	parse_texture(const char *map_path, const char *rest,
		char **out)
{
	char		tok[MAP_MAX_PATH];
	const char	*end;
	size_t		len;

	rest = skip_ws(rest);
	end = rest;
	while (*end && *end != ' ' && *end != '\t')
		end++;
	len = (size_t)(end - rest);
	if (len == 0 || !is_blank(end))
		return (MAP_ERR_TEXTURE_SYNTAX);
	if (len >= sizeof(tok))
		return (MAP_ERR_TEXTURE_MISSING);
	memcpy(tok, rest, len);
	tok[len] = '\0';
	return (resolve_texture(map_path, tok, out));
}

/* "R,G,B": three 0-255 decimals, optional blanks around each number. */
static t_map_error	parse_color(const char *s, uint32_t *out)
{
	int	i;
	int	digits;
	int	value;

	*out = 0;
	i = 0;
	while (i < 3)
	{
		s = skip_ws(s);
		value = 0;
		digits = 0;
		while (*s >= '0' && *s <= '9' && digits < 4)
		{
			value = value * 10 + (*s++ - '0');
			digits++;
		}
		if (digits == 0 || digits > 3 || value > 255)
			return (MAP_ERR_COLOR);
		*out = (*out << 8) | (uint32_t)value;
		s = skip_ws(s);
		if (i < 2 && *s++ != ',')
			return (MAP_ERR_COLOR);
		i++;
	}
	if (*s != '\0')
		return (MAP_ERR_COLOR);
	return (MAP_OK);
}

static int	element_id(const char *s, const char **rest)
{
	int		i;
	size_t	len;

	i = 0;
	while (i < ELEM_COUNT)
	{
		len = strlen(g_ids[i]);
		if (strncmp(s, g_ids[i], len) == 0
			&& (s[len] == ' ' || s[len] == '\t' || s[len] == '\0'))
		{
			*rest = s + len;
			return (i);
		}
		i++;
	}
	return (-1);
}

static t_map_error	parse_element(const char *map_path, const char *line,
		t_map *m, bool *seen)
{
	const char	*s;
	const char	*rest;
	int			id;

	s = skip_ws(line);
	id = element_id(s, &rest);
	if (id < 0)
	{
		if (strchr("01NSEW", *s) != NULL)
			return (MAP_ERR_MISSING_ELEMENT);
		return (MAP_ERR_UNKNOWN_ELEMENT);
	}
	if (seen[id])
		return (MAP_ERR_DUPLICATE_ELEMENT);
	seen[id] = true;
	if (id == ELEM_F)
		return (parse_color(rest, &m->floor_rgb));
	if (id == ELEM_C)
		return (parse_color(rest, &m->ceil_rgb));
	return (parse_texture(map_path, rest, &m->tex[id]));
}

t_map_error	map_parse_header(const char *map_path, t_lines *l, t_map *m,
		int *next)
{
	bool		seen[ELEM_COUNT];
	int			found;
	int			i;
	t_map_error	err;

	memset(seen, 0, sizeof(seen));
	found = 0;
	i = 0;
	while (i < l->count && found < ELEM_COUNT)
	{
		if (!is_blank(l->line[i]))
		{
			err = parse_element(map_path, l->line[i], m, seen);
			if (err != MAP_OK)
				return (map_fail(m, err, i + 1, 0));
			found++;
		}
		i++;
	}
	if (found == 0)
		return (MAP_ERR_EMPTY);
	if (found < ELEM_COUNT)
		return (MAP_ERR_MISSING_ELEMENT);
	*next = i;
	return (MAP_OK);
}
