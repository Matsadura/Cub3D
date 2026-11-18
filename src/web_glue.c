#include "web.h"

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
# define EXPORT EMSCRIPTEN_KEEPALIVE
#else
# define EXPORT
#endif

#define HELD_MASK	(ACT_BIT(ACT_FORWARD) | ACT_BIT(ACT_BACK) \
	| ACT_BIT(ACT_STRAFE_L) | ACT_BIT(ACT_STRAFE_R) | ACT_BIT(ACT_TURN_L) \
	| ACT_BIT(ACT_TURN_R) | ACT_BIT(ACT_SPRINT))

typedef struct s_web
{
	t_platform	*platform;
	const t_game	*game;
	t_input		pending;
	uint32_t	held;
	float		look;
	bool		new_game;
	bool		auto_pause;
}	t_web;

static t_web	g_web;

EXPORT void	web_action(int action)
{
	static const t_action	map[] = {ACT_CONFIRM, ACT_RESTART, ACT_PAUSE,
		ACT_LIGHT, ACT_MAP};

	if (action >= 0 && action < (int)(sizeof(map) / sizeof(map[0])))
		input_push(&g_web.pending, map[action]);
	else if (action == WEB_NEW_GAME)
		g_web.new_game = true;
	else if (action == WEB_AUTO_PAUSE)
		g_web.auto_pause = true;
	else if (action == WEB_TOGGLE_MUTE)
		input_push(&g_web.pending, ACT_MUTE);
}

EXPORT void	web_set_held(int mask)
{
	g_web.held = (uint32_t)mask & HELD_MASK;
}

EXPORT void	web_look(float dx)
{
	g_web.look += dx;
}

EXPORT int	web_is_muted(void)
{
	return (g_web.platform != NULL && platform_is_muted(g_web.platform));
}

EXPORT double	web_player_angle(void)
{
	if (g_web.game == NULL)
		return (-1.0);
	return (g_web.game->player.angle * 180.0 / PI_D);
}

EXPORT int	web_game_state(void)
{
	if (g_web.game == NULL)
		return (-1);
	return ((int)g_web.game->state);
}

void	web_init(t_platform *p, const t_game *g)
{
	g_web.platform = p;
	g_web.game = g;
}

void	web_apply(t_input *in, t_game *g)
{
	int	i;

	i = 0;
	while (i < g_web.pending.count)
		input_push(in, g_web.pending.actions[i++]);
	g_web.pending.count = 0;
	in->held |= g_web.held;
	in->mouse_dx += g_web.look;
	g_web.look = 0.0f;
	if (g_web.new_game)
		game_new_run(g);
	if (g_web.auto_pause && g->state == GAME_PLAYING)
		g->state = GAME_PAUSED;
	g_web.new_game = false;
	g_web.auto_pause = false;
}
