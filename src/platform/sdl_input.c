#include "sdl_internal.h"

#ifdef __EMSCRIPTEN__
# include <emscripten.h>

/* The page requests pointer lock on click (it must happen inside a user
** gesture); mouse look is active while the canvas holds the lock. */
EM_JS(int, js_pointer_locked, (void), {
	return document.pointerLockElement === Module.canvas ? 1 : 0;
});
#endif

void	input_push(t_input *in, t_action action)
{
	if (in->count < INPUT_MAX_ACTIONS)
		in->actions[in->count++] = action;
}

bool	input_pressed(const t_input *in, t_action action)
{
	int	i;

	i = 0;
	while (i < in->count)
		if (in->actions[i++] == action)
			return (true);
	return (false);
}

bool	input_held(const t_input *in, t_action action)
{
	return ((in->held & ACT_BIT(action)) != 0);
}

void	platform_capture_mouse(t_platform *p, bool on)
{
#ifdef __EMSCRIPTEN__
	p->mouse_wanted = on;
	return ;
#endif
	p->mouse_wanted = on;
	p->mouse_captured = on && SDL_SetRelativeMouseMode(
			on ? SDL_TRUE : SDL_FALSE) == 0;
	if (!on)
		SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void	map_key(t_input *in, SDL_Keycode key)
{
	if (key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE)
		input_push(in, ACT_CONFIRM);
	else if (key == SDLK_r)
		input_push(in, ACT_RESTART);
	else if (key == SDLK_p)
		input_push(in, ACT_PAUSE);
	else if (key == SDLK_f)
		input_push(in, ACT_LIGHT);
	else if (key == SDLK_TAB)
		input_push(in, ACT_MAP);
	else if (key == SDLK_m)
		input_push(in, ACT_MUTE);
#ifdef __EMSCRIPTEN__
	else if (key == SDLK_ESCAPE)
		input_push(in, ACT_PAUSE);
#else
	else if (key == SDLK_ESCAPE)
		input_push(in, ACT_QUIT);
#endif
}

/* Movement uses the live keyboard state, so opposing keys never stick. */
static uint32_t	held_keys(void)
{
	const Uint8	*k;
	uint32_t	held;

	k = SDL_GetKeyboardState(NULL);
	held = 0;
	if (k[SDL_SCANCODE_W] || k[SDL_SCANCODE_UP])
		held |= ACT_BIT(ACT_FORWARD);
	if (k[SDL_SCANCODE_S] || k[SDL_SCANCODE_DOWN])
		held |= ACT_BIT(ACT_BACK);
	if (k[SDL_SCANCODE_A])
		held |= ACT_BIT(ACT_STRAFE_L);
	if (k[SDL_SCANCODE_D])
		held |= ACT_BIT(ACT_STRAFE_R);
	if (k[SDL_SCANCODE_LEFT])
		held |= ACT_BIT(ACT_TURN_L);
	if (k[SDL_SCANCODE_RIGHT])
		held |= ACT_BIT(ACT_TURN_R);
	if (k[SDL_SCANCODE_LSHIFT] || k[SDL_SCANCODE_RSHIFT])
		held |= ACT_BIT(ACT_SPRINT);
	return (held);
}

static void	handle_window(t_platform *p, const SDL_Event *ev)
{
	if (ev->window.event == SDL_WINDOWEVENT_FOCUS_LOST && p->mouse_captured)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		p->mouse_captured = false;
	}
}

void	platform_poll_input(t_platform *p, t_input *in)
{
	SDL_Event	ev;

	in->count = 0;
	in->mouse_dx = 0.0f;
#ifdef __EMSCRIPTEN__
	p->mouse_captured = js_pointer_locked() != 0;
#endif
	in->quit_requested = false;
	in->targets_reset = false;
	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_QUIT)
			in->quit_requested = true;
		else if (ev.type == SDL_KEYDOWN && !ev.key.repeat)
			map_key(in, ev.key.keysym.sym);
		else if (ev.type == SDL_MOUSEMOTION && p->mouse_captured)
			in->mouse_dx += (float)ev.motion.xrel;
#ifndef __EMSCRIPTEN__
		else if (ev.type == SDL_MOUSEBUTTONDOWN && p->mouse_wanted
			&& !p->mouse_captured)
			platform_capture_mouse(p, true);
#endif
		else if (ev.type == SDL_WINDOWEVENT)
			handle_window(p, &ev);
		else if (ev.type == SDL_RENDER_TARGETS_RESET
			|| ev.type == SDL_RENDER_DEVICE_RESET)
			in->targets_reset = true;
	}
	in->held = held_keys();
}
