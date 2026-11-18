#include "sdl_internal.h"

#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __EMSCRIPTEN__
/* In the browser the canvas keeps the logical size; CSS scales it. */
# define WINDOW_FLAGS 0
#else
# define WINDOW_FLAGS (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)
#endif

const char	*platform_error(void)
{
	return (SDL_GetError());
}

/* Natively: the largest scale that fits ~85% of the usable desktop. */
static void	initial_window_size(int w, int h, int *out_w, int *out_h)
{
	SDL_Rect	bounds;
	double		scale;
	double		sy;

	scale = 1.0;
#ifndef __EMSCRIPTEN__
	if (SDL_GetDisplayUsableBounds(0, &bounds) == 0
		&& bounds.w > 0 && bounds.h > 0)
	{
		scale = bounds.w * 0.85 / w;
		sy = bounds.h * 0.85 / h;
		if (sy < scale)
			scale = sy;
		if (scale < 1.0)
			scale = 1.0;
	}
#else
	(void)bounds;
	(void)sy;
#endif
	*out_w = (int)(w * scale);
	*out_h = (int)(h * scale);
}

static bool	create_window(t_platform *p, const char *title)
{
	SDL_RendererInfo	info;
	int					w;
	int					h;

	initial_window_size(p->logical_w, p->logical_h, &w, &h);
	p->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, w, h, WINDOW_FLAGS);
	if (p->window == NULL)
		return (false);
	p->renderer = SDL_CreateRenderer(p->window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (p->renderer == NULL)
		p->renderer = SDL_CreateRenderer(p->window, -1,
				SDL_RENDERER_SOFTWARE);
	if (p->renderer == NULL)
		return (false);
	if (SDL_GetRendererInfo(p->renderer, &info) == 0)
		p->vsync = (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0;
	SDL_SetRenderDrawBlendMode(p->renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(p->renderer, p->logical_w, p->logical_h);
	return (true);
}

static bool	create_framebuffer(t_platform *p)
{
	p->frame = calloc((size_t)p->logical_w * (size_t)p->logical_h,
			sizeof(uint32_t));
	if (p->frame == NULL)
		return (SDL_SetError("out of memory"), false);
	p->frame_tex = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, p->logical_w, p->logical_h);
	return (p->frame_tex != NULL);
}

t_platform	*platform_create(const char *title, int logical_w, int logical_h)
{
	t_platform	*p;

	p = calloc(1, sizeof(*p));
	if (p == NULL)
		return (NULL);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		free(p);
		return (NULL);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	p->logical_w = logical_w;
	p->logical_h = logical_h;
	p->img_ready = (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != 0;
	if (!p->img_ready || !create_window(p, title) || !create_framebuffer(p))
	{
		platform_destroy(p);
		return (NULL);
	}
	return (p);
}

void	platform_destroy(t_platform *p)
{
	int	i;

	if (p == NULL)
		return ;
	if (p->mouse_captured)
		SDL_SetRelativeMouseMode(SDL_FALSE);
	i = 0;
	while (i < MAX_TEXTURES)
	{
		if (p->textures[i] != NULL)
			SDL_DestroyTexture(p->textures[i]);
		i++;
	}
	if (p->frame_tex != NULL)
		SDL_DestroyTexture(p->frame_tex);
	free(p->frame);
	if (p->renderer != NULL)
		SDL_DestroyRenderer(p->renderer);
	if (p->window != NULL)
		SDL_DestroyWindow(p->window);
	if (p->img_ready)
		IMG_Quit();
	SDL_Quit();
	free(p);
}

bool	platform_locate_assets(void)
{
#ifdef __EMSCRIPTEN__
	return (true);
#else
	char	*base;
	bool	ok;

	if (access("assets/textures", R_OK) == 0)
		return (true);
	base = SDL_GetBasePath();
	if (base == NULL)
		return (false);
	ok = (chdir(base) == 0 && access("assets/textures", R_OK) == 0);
	SDL_free(base);
	return (ok);
#endif
}

double	platform_time_seconds(void)
{
	return ((double)SDL_GetPerformanceCounter()
		/ (double)SDL_GetPerformanceFrequency());
}

void	platform_limit_fps(t_platform *p, double frame_start, int max_fps)
{
#ifdef __EMSCRIPTEN__
	(void)p;
	(void)frame_start;
	(void)max_fps;
#else
	double	remaining;

	if (p->vsync || max_fps <= 0)
		return ;
	remaining = 1.0 / max_fps - (platform_time_seconds() - frame_start);
	if (remaining > 0.001)
		SDL_Delay((Uint32)(remaining * 1000.0));
#endif
}
