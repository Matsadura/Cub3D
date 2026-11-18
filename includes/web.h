#ifndef WEB_H
# define WEB_H

# include "game.h"
# include "platform.h"

/*
** Browser glue. The page calls the exported web_* functions (buttons,
** touch stick, touch look); they only queue input, which web_apply() feeds
** into the next tick, so JavaScript never re-enters the game mid-frame.
** Natively the functions exist but nothing calls them.
*/

/* web_action() codes, mirrored in web/shell.html */
# define WEB_CONFIRM		0
# define WEB_RESTART		1
# define WEB_PAUSE			2
# define WEB_LIGHT			3
# define WEB_MAP			4
# define WEB_NEW_GAME		7
# define WEB_AUTO_PAUSE		8
# define WEB_TOGGLE_MUTE	9

void	web_action(int action);
/* Bitmask of ACT_BIT(ACT_FORWARD .. ACT_SPRINT) held by touch controls. */
void	web_set_held(int mask);
/* Horizontal look from a touch drag, in mouse pixels. */
void	web_look(float dx);
int		web_is_muted(void);
/* Test hook for browser automation: player angle in degrees, state index. */
double	web_player_angle(void);
int		web_game_state(void);

void	web_init(t_platform *p, const t_game *g);
void	web_apply(t_input *in, t_game *g);

#endif
