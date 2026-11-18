#ifndef PLATFORM_H
# define PLATFORM_H

# include <stdbool.h>
# include <stdint.h>

/*
** Hardware / OS abstraction. Everything that touches SDL lives behind this
** header (implementation in src/platform/). Game logic in src/core and
** src/map never includes SDL.
*/

typedef struct s_platform	t_platform;

/* Handle into the platform's sound cache. */
typedef int					t_sound;
# define SOUND_NONE			(-1)

/* Handle into the platform's GPU texture cache (overlays, HUD sprites). */
typedef int					t_tex;
# define TEX_NONE			(-1)

/* CPU-side image (0xAARRGGBB pixels), used by the software raycaster. */
typedef struct s_image
{
	int			w;
	int			h;
	uint32_t	*px;
}	t_image;

typedef enum e_action
{
	ACT_FORWARD,
	ACT_BACK,
	ACT_STRAFE_L,
	ACT_STRAFE_R,
	ACT_TURN_L,
	ACT_TURN_R,
	ACT_SPRINT,
	ACT_LIGHT,
	ACT_CONFIRM,
	ACT_RESTART,
	ACT_PAUSE,
	ACT_MAP,
	ACT_MUTE,
	ACT_QUIT,
	ACT_COUNT
}	t_action;

# define INPUT_MAX_ACTIONS	16
# define ACT_BIT(a)			(1u << (a))

typedef struct s_input
{
	/* Keys pressed this frame (OS key-repeat ignored). */
	t_action	actions[INPUT_MAX_ACTIONS];
	int			count;
	/* Bitmask of ACT_BIT(action) currently held down. */
	uint32_t	held;
	/* Horizontal mouse motion this frame in pixels (mouse look). */
	float		mouse_dx;
	bool		quit_requested;
	/* GPU render targets were lost; cached layers must be rebuilt. */
	bool		targets_reset;
}	t_input;

t_platform	*platform_create(const char *title, int logical_w, int logical_h);
void		platform_destroy(t_platform *p);
const char	*platform_error(void);

/* Natively: chdir next to the binary if ./assets is not reachable. */
bool		platform_locate_assets(void);

void		platform_poll_input(t_platform *p, t_input *in);
void		input_push(t_input *in, t_action action);
bool		input_pressed(const t_input *in, t_action action);
bool		input_held(const t_input *in, t_action action);
/* Relative mouse mode (mouse look). Natively on by default. */
void		platform_capture_mouse(t_platform *p, bool on);

/* Best escape time in milliseconds (0 = none): localStorage in the
** browser, session-only natively. */
int			platform_load_best(void);
void		platform_save_best(int ms);

double		platform_time_seconds(void);
/* Native-only frame cap when vsync is unavailable; no-op in the browser. */
void		platform_limit_fps(t_platform *p, double frame_start, int max_fps);

/* Decodes an image file into CPU pixels. */
bool		platform_load_image(const char *path, t_image *out);
void		platform_free_image(t_image *img);

/* logical_w * logical_h pixels for the software renderer. */
uint32_t	*platform_framebuffer(t_platform *p);
/* Uploads the framebuffer and draws it as the frame's background. */
void		platform_blit_framebuffer(t_platform *p);

t_tex		platform_load_texture(t_platform *p, const char *path);
void		platform_free_texture(t_platform *p, t_tex tex);
void		platform_clear(t_platform *p, uint32_t rgba);
/* tint 0xRRGGBBAA (white = unchanged). */
void		platform_draw_ex(t_platform *p, t_tex tex, const int rect[4],
				uint32_t tint, bool flip_h);
void		platform_fill_rect(t_platform *p, int x, int y, int w, int h,
				uint32_t rgba);
void		platform_present(t_platform *p);
/* Saves the last presented frame as a BMP (testing / screenshots). */
bool		platform_save_screenshot(t_platform *p, const char *path);

/*
** Audio: a small software mixer (several effect voices plus one looping
** music voice). Every call is a safe no-op when no audio device could be
** opened, so the game simply runs silently.
*/
bool		platform_audio_open(t_platform *p);
t_sound		platform_load_sound(t_platform *p, const char *path);
/* volume: 0.0 .. 1.0 */
void		platform_play_sound(t_platform *p, t_sound s, float volume);
/* Loops s; keeps playing (no restart) if s is already the current track. */
void		platform_play_music(t_platform *p, t_sound s, float volume);
void		platform_stop_music(t_platform *p);
void		platform_pause_music(t_platform *p, bool paused);
void		platform_set_muted(t_platform *p, bool muted);
bool		platform_is_muted(const t_platform *p);
/* Mute preference: localStorage in the browser, session-only natively. */
bool		platform_load_muted(void);
void		platform_save_muted(bool muted);

#endif
