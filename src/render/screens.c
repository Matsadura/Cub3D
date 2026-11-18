#include "render.h"

#include <math.h>
#include <stdio.h>

#define WHITE		0xE8E0D0FFu
#define DIM			0x8A8478FFu
#define BLOOD		0xC02018FFu
#define GREEN		0x6CE08AFFu
#define SHADE		0x000000B0u
#define SHADE_FULL	0x000000E0u

static void	center(t_platform *p, const char *s, int y, int scale, uint32_t c)
{
	draw_text_centered(p, s, (const int [2]){VIEW_W / 2, y}, scale, c);
}

/* Prompts blink slowly so the screen never looks frozen. */
static uint32_t	blink(const t_game *g, uint32_t c)
{
	if (fmod(g->state_timer, 1.2) > 0.85)
		return (c & 0xFFFFFF40u);
	return (c);
}

static void	draw_title(t_platform *p, const t_game *g)
{
	char	buf[48];

	platform_fill_rect(p, 0, 0, VIEW_W, VIEW_H, SHADE);
	center(p, "A CUB3D HORROR ESCAPE", 70, 1, DIM);
	center(p, GAME_TITLE_TEXT, 88, 7, BLOOD);
	center(p, "FIND THE KEYS. REACH THE EXIT. STAY OUT OF ITS SIGHT.", 160,
		1, WHITE);
	center(p, "WASD MOVE   MOUSE/ARROWS LOOK   SHIFT SPRINT", 190, 1, DIM);
	center(p, "F FLASHLIGHT   TAB MAP   P PAUSE   M MUTE", 204, 1, DIM);
	center(p, "PRESS ENTER", 250, 2, blink(g, WHITE));
	if (g->best_time > 0.0)
	{
		format_time(g->best_time, buf, sizeof(buf));
		draw_text_centered(p, "BEST ESCAPE", (const int [2]){VIEW_W / 2, 300},
			1, DIM);
		center(p, buf, 312, 2, GREEN);
	}
}

static void	draw_pause(t_platform *p, const t_game *g)
{
	(void)g;
	platform_fill_rect(p, 0, 0, VIEW_W, VIEW_H, SHADE);
	center(p, "PAUSED", 120, 4, WHITE);
	center(p, "P OR ENTER TO RESUME   R RESTART LEVEL", 180, 1, DIM);
	center(p, "WASD MOVE   MOUSE/ARROWS LOOK   SHIFT SPRINT", 200, 1, DIM);
	center(p, "F FLASHLIGHT   TAB MAP   M MUTE", 214, 1, DIM);
}

static void	draw_caught_text(t_platform *p, const t_game *g)
{
	char	buf[48];

	if (g->state_timer < 1.2)
		return ;
	platform_fill_rect(p, 0, 0, VIEW_W, VIEW_H, SHADE);
	center(p, "IT GOT YOU", 130, 4, BLOOD);
	snprintf(buf, sizeof(buf), "%d %s LEFT", g->lives - 1,
		g->lives - 1 == 1 ? "LIFE" : "LIVES");
	center(p, buf, 180, 2, WHITE);
}

static void	draw_end(t_platform *p, const t_game *g, const char *title,
		uint32_t color)
{
	char	name[48];
	char	buf[64];

	platform_fill_rect(p, 0, 0, VIEW_W, VIEW_H, SHADE_FULL);
	center(p, title, 110, 4, color);
	if (g->state == GAME_ESCAPED)
	{
		level_title(g, name, sizeof(name));
		snprintf(buf, sizeof(buf), "YOU ESCAPED THE %s", name);
		center(p, buf, 160, 2, WHITE);
	}
	format_time(g->run_time, name, sizeof(name));
	snprintf(buf, sizeof(buf), "TIME %s", name);
	if (g->state != GAME_OVER)
		center(p, buf, 190, 2, DIM);
	if (g->state == GAME_VICTORY)
	{
		format_time(g->best_time, name, sizeof(name));
		snprintf(buf, sizeof(buf), "BEST %s", name);
		center(p, buf, 214, 2, GREEN);
	}
	if (g->state == GAME_OVER)
		center(p, "THE DARK KEEPS WHAT IT CATCHES", 160, 1, DIM);
	center(p, g->state == GAME_OVER ? "PRESS ENTER TO TRY AGAIN"
		: "PRESS ENTER", 270, 2, blink(g, WHITE));
}

void	draw_screens(t_platform *p, const t_game *g)
{
	if (g->state == GAME_TITLE)
		draw_title(p, g);
	else if (g->state == GAME_PAUSED)
		draw_pause(p, g);
	else if (g->state == GAME_CAUGHT)
		draw_caught_text(p, g);
	else if (g->state == GAME_ESCAPED)
		draw_end(p, g, g->level_index + 1 < g->level_count
			? "ESCAPED" : "FREEDOM", GREEN);
	else if (g->state == GAME_OVER)
		draw_end(p, g, "THE DARK TOOK YOU", BLOOD);
	else if (g->state == GAME_VICTORY)
		draw_end(p, g, "YOU MADE IT OUT", GREEN);
}
