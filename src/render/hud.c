#include "render.h"

#include <stdio.h>
#include <string.h>

#define TEXT		0xD8D0C0FFu
#define TEXT_DIM	0x8A8478FFu
#define KEY_GOLD	0xF0C040FFu
#define BAR_BG		0x00000090u
#define BATT_OK		0x7FE08AFFu
#define BATT_LOW	0xE05040FFu
#define STAM_OK		0xC8C0B0FFu
#define STAM_OUT	0x806040FFu
#define LIFE		0xB02020FFu
#define INTRO_TIME	3.0

void	format_time(double seconds, char *buf, size_t size)
{
	int	s;

	s = (int)seconds;
	snprintf(buf, size, "%d:%02d", s / 60, s % 60);
}

/* "assets/maps/campaign/03_the_deep.cub" -> "THE DEEP" */
void	level_title(const t_game *g, char *buf, size_t size)
{
	const char	*s;
	size_t		i;

	s = strrchr(g->levels[g->level_index], '/');
	s = (s == NULL) ? g->levels[g->level_index] : s + 1;
	if (s[0] >= '0' && s[0] <= '9' && s[1] >= '0' && s[1] <= '9'
		&& s[2] == '_')
		s += 3;
	i = 0;
	while (s[i] && strcmp(s + i, ".cub") != 0 && i + 1 < size)
	{
		buf[i] = (s[i] == '_') ? ' ' : s[i];
		if (buf[i] >= 'a' && buf[i] <= 'z')
			buf[i] -= 32;
		i++;
	}
	buf[i] = '\0';
}

static void	bar(t_platform *p, const int pos[2], double v, uint32_t color)
{
	platform_fill_rect(p, pos[0], pos[1], 82, 8, BAR_BG);
	platform_fill_rect(p, pos[0] + 1, pos[1] + 1, (int)(80 * v), 6, color);
}

static void	draw_status(t_platform *p, const t_game *g)
{
	char	buf[48];
	int		i;

	draw_text(p, "LIGHT", (const int [2]){12, VIEW_H - 34}, 1, TEXT_DIM);
	bar(p, (const int [2]){50, VIEW_H - 35}, g->battery,
		g->battery < BATTERY_LOW ? BATT_LOW : BATT_OK);
	draw_text(p, "BREATH", (const int [2]){12, VIEW_H - 20}, 1, TEXT_DIM);
	bar(p, (const int [2]){50, VIEW_H - 21}, g->stamina,
		g->exhausted ? STAM_OUT : STAM_OK);
	if (g->free_roam)
		snprintf(buf, sizeof(buf), "EXPLORE");
	else
		snprintf(buf, sizeof(buf), "KEYS %d/%d", g->keys_held,
			g->map.key_count);
	draw_text(p, buf, (const int [2]){VIEW_W - 12 - text_width(buf, 2), 12},
		2, (g->keys_held >= g->map.key_count && !g->free_roam)
		? KEY_GOLD : TEXT);
	format_time(g->run_time, buf, sizeof(buf));
	draw_text(p, buf, (const int [2]){VIEW_W - 12 - text_width(buf, 1), 34},
		1, TEXT_DIM);
	i = -1;
	while (++i < g->lives)
		platform_fill_rect(p, VIEW_W - 18 - i * 12, 46, 8, 8, LIFE);
}

static const char	*message_text(const t_game *g, char *buf, size_t size)
{
	static const char	*text[] = {"", NULL, "YOU FOUND A KEY",
		"ALL KEYS FOUND - FIND THE EXIT", "FRESH BATTERY",
		"YOUR LIGHT DIED", "IT HEARD YOU", "OUT OF BREATH"};

	if (g->message == MSG_LOCKED)
	{
		snprintf(buf, size, "LOCKED - %d KEY%s MISSING",
			g->map.key_count - g->keys_held,
			g->map.key_count - g->keys_held == 1 ? "" : "S");
		return (buf);
	}
	return (text[g->message]);
}

static void	draw_messages(t_platform *p, const t_game *g)
{
	char		buf[64];
	char		name[48];
	uint32_t	alpha;

	if (g->level_time < INTRO_TIME)
	{
		alpha = (uint32_t)(255 * (1.0 - g->level_time / INTRO_TIME));
		level_title(g, name, sizeof(name));
		snprintf(buf, sizeof(buf), "LEVEL %d", g->level_index + 1);
		draw_text_centered(p, buf, (const int [2]){VIEW_W / 2, 92}, 2,
			(TEXT_DIM & 0xFFFFFF00u) | alpha);
		draw_text_centered(p, name, (const int [2]){VIEW_W / 2, 112}, 4,
			(TEXT & 0xFFFFFF00u) | alpha);
	}
	if (g->message != MSG_NONE && g->message_timer > 0.0)
	{
		alpha = (uint32_t)(255 * (g->message_timer > 1.0 ? 1.0
					: g->message_timer));
		draw_text_centered(p, message_text(g, buf, sizeof(buf)),
			(const int [2]){VIEW_W / 2, VIEW_H - 70}, 2,
			((g->message == MSG_SPOTTED ? 0xE04030FFu : TEXT) & 0xFFFFFF00u)
			| alpha);
	}
}

void	draw_hud(t_platform *p, const t_game *g)
{
	if (g->state != GAME_PLAYING && g->state != GAME_PAUSED)
		return ;
	draw_status(p, g);
	draw_messages(p, g);
}
