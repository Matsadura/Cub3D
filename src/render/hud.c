#include "render.h"

#define WHITE	0xFFFFFFFFu
#define DIM		0x000000A0u
#define GREY	0xB0B0B0FFu

static void	draw_crosshair(t_platform *p)
{
	platform_fill_rect(p, VIEW_W / 2 - 4, VIEW_H / 2, 3, 1, 0xFFFFFF90u);
	platform_fill_rect(p, VIEW_W / 2 + 2, VIEW_H / 2, 3, 1, 0xFFFFFF90u);
	platform_fill_rect(p, VIEW_W / 2, VIEW_H / 2 - 4, 1, 3, 0xFFFFFF90u);
	platform_fill_rect(p, VIEW_W / 2, VIEW_H / 2 + 2, 1, 3, 0xFFFFFF90u);
}

static void	draw_pause(t_platform *p)
{
	platform_fill_rect(p, 0, 0, VIEW_W, VIEW_H, DIM);
	draw_text_centered(p, "PAUSED", (const int [2]){VIEW_W / 2, 130}, 4,
		WHITE);
	draw_text_centered(p, "P OR ENTER TO RESUME",
		(const int [2]){VIEW_W / 2, 180}, 2, GREY);
	draw_text_centered(p, "WASD MOVE  ARROWS/MOUSE TURN  TAB MAP  R RESTART",
		(const int [2]){VIEW_W / 2, 210}, 1, GREY);
}

void	draw_overlays(t_platform *p, const t_game *g)
{
	if (g->state == GAME_PLAYING)
		draw_crosshair(p);
	else if (g->state == GAME_PAUSED)
		draw_pause(p);
}
