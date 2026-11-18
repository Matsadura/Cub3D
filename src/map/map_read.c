#include "map_internal.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* "maps/x.cub" ok; ".cub", "maps/.cub", "x.cub.txt", "x.CUB" rejected. */
static bool	valid_extension(const char *path)
{
	const char	*base;

	base = strrchr(path, '/');
	if (base == NULL)
		base = path;
	else
		base++;
	return (has_suffix(base, ".cub"));
}

static t_map_error	read_all(int fd, size_t size, char **out)
{
	char	*buf;
	size_t	len;
	ssize_t	n;

	buf = malloc(size + 1);
	if (buf == NULL)
		return (MAP_ERR_NOMEM);
	len = 0;
	n = 1;
	while (len <= size && n > 0)
	{
		n = read(fd, buf + len, size + 1 - len);
		if (n < 0 && errno == EINTR)
			n = 1;
		else if (n > 0)
			len += (size_t)n;
	}
	if (n < 0 || len > size)
	{
		free(buf);
		return (n < 0 ? MAP_ERR_READ : MAP_ERR_FILE_TOO_BIG);
	}
	buf[len] = '\0';
	if (strlen(buf) != len)
	{
		free(buf);
		return (MAP_ERR_BINARY);
	}
	*out = buf;
	return (MAP_OK);
}

/* Opens, checks and reads the whole file; the fd is always closed. */
static t_map_error	load_file(const char *path, char **out)
{
	struct stat	st;
	int			fd;
	t_map_error	err;

	if (!valid_extension(path))
		return (MAP_ERR_EXTENSION);
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (MAP_ERR_OPEN);
	err = MAP_OK;
	if (fstat(fd, &st) != 0)
		err = MAP_ERR_READ;
	else if (!S_ISREG(st.st_mode))
		err = MAP_ERR_NOT_FILE;
	else if (st.st_size > MAP_MAX_FILE_SIZE)
		err = MAP_ERR_FILE_TOO_BIG;
	else
		err = read_all(fd, MAP_MAX_FILE_SIZE, out);
	close(fd);
	return (err);
}

static int	count_lines(const char *s)
{
	int	n;

	n = 1;
	while (*s)
		if (*s++ == '\n')
			n++;
	return (n);
}

/* Splits buf in place into lines; LF and CRLF endings are both accepted. */
static t_map_error	split_lines(t_lines *l)
{
	char	*s;
	char	*nl;
	size_t	len;
	int		i;

	l->line = malloc(sizeof(char *) * (count_lines(l->buf) + 1));
	if (l->line == NULL)
		return (MAP_ERR_NOMEM);
	s = l->buf;
	i = 0;
	while (s != NULL)
	{
		nl = strchr(s, '\n');
		if (nl != NULL)
			*nl = '\0';
		len = strlen(s);
		if (len > 0 && s[len - 1] == '\r')
			s[len - 1] = '\0';
		l->line[i++] = s;
		s = nl ? nl + 1 : NULL;
	}
	if (i > 0 && l->line[i - 1][0] == '\0')
		i--;
	l->line[i] = NULL;
	l->count = i;
	return (MAP_OK);
}

t_map_error	map_read_lines(const char *path, t_lines *out)
{
	t_map_error	err;

	memset(out, 0, sizeof(*out));
	err = load_file(path, &out->buf);
	if (err == MAP_OK)
		err = split_lines(out);
	if (err != MAP_OK)
		lines_free(out);
	return (err);
}

void	lines_free(t_lines *l)
{
	free(l->buf);
	free(l->line);
	memset(l, 0, sizeof(*l));
}
