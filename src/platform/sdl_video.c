#include "sdl_internal.h"

#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>

bool	platform_load_image(const char *path, t_image *out)
{
	SDL_Surface	*raw;
	SDL_Surface	*conv;
	int			y;

	memset(out, 0, sizeof(*out));
	raw = IMG_Load(path);
	if (raw == NULL)
		return (false);
	conv = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(raw);
	if (conv == NULL)
		return (false);
	out->px = malloc((size_t)conv->w * (size_t)conv->h * sizeof(uint32_t));
	if (out->px != NULL && SDL_LockSurface(conv) == 0)
	{
		out->w = conv->w;
		out->h = conv->h;
		y = -1;
		while (++y < conv->h)
			memcpy(out->px + (size_t)y * conv->w,
				(uint8_t *)conv->pixels + (size_t)y * conv->pitch,
				(size_t)conv->w * sizeof(uint32_t));
		SDL_UnlockSurface(conv);
	}
	SDL_FreeSurface(conv);
	if (out->w == 0)
		platform_free_image(out);
	return (out->px != NULL);
}

void	platform_free_image(t_image *img)
{
	free(img->px);
	memset(img, 0, sizeof(*img));
}

uint32_t	*platform_framebuffer(t_platform *p)
{
	return (p->frame);
}

void	platform_blit_framebuffer(t_platform *p)
{
	SDL_UpdateTexture(p->frame_tex, NULL, p->frame,
		p->logical_w * (int)sizeof(uint32_t));
	SDL_RenderCopy(p->renderer, p->frame_tex, NULL, NULL);
}

t_tex	platform_load_texture(t_platform *p, const char *path)
{
	SDL_Texture	*tex;
	int			i;

	tex = IMG_LoadTexture(p->renderer, path);
	if (tex == NULL)
		return (TEX_NONE);
	i = 0;
	while (i < MAX_TEXTURES && p->textures[i] != NULL)
		i++;
	if (i == MAX_TEXTURES)
	{
		SDL_DestroyTexture(tex);
		SDL_SetError("texture cache full");
		return (TEX_NONE);
	}
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	p->textures[i] = tex;
	return (i);
}

void	platform_free_texture(t_platform *p, t_tex tex)
{
	if (tex < 0 || tex >= MAX_TEXTURES || p->textures[tex] == NULL)
		return ;
	SDL_DestroyTexture(p->textures[tex]);
	p->textures[tex] = NULL;
}

static void	set_color(t_platform *p, uint32_t rgba)
{
	SDL_SetRenderDrawColor(p->renderer, (rgba >> 24) & 0xFF,
		(rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
}

void	platform_clear(t_platform *p, uint32_t rgba)
{
	set_color(p, rgba);
	SDL_RenderClear(p->renderer);
}

void	platform_draw_ex(t_platform *p, t_tex tex, const int rect[4],
		uint32_t tint, bool flip_h)
{
	SDL_Texture	*t;
	SDL_Rect	dst;

	if (tex < 0 || tex >= MAX_TEXTURES || p->textures[tex] == NULL)
		return ;
	t = p->textures[tex];
	dst = (SDL_Rect){rect[0], rect[1], rect[2], rect[3]};
	SDL_SetTextureColorMod(t, (tint >> 24) & 0xFF, (tint >> 16) & 0xFF,
		(tint >> 8) & 0xFF);
	SDL_SetTextureAlphaMod(t, tint & 0xFF);
	SDL_RenderCopyEx(p->renderer, t, NULL, &dst, 0.0, NULL,
		flip_h ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void	platform_fill_rect(t_platform *p, int x, int y, int w, int h,
		uint32_t rgba)
{
	SDL_Rect	r;

	r = (SDL_Rect){x, y, w, h};
	set_color(p, rgba);
	SDL_RenderFillRect(p->renderer, &r);
}

void	platform_present(t_platform *p)
{
	SDL_RenderPresent(p->renderer);
}

/* Reads back the renderer's output (call before the next present). */
bool	platform_save_screenshot(t_platform *p, const char *path)
{
	SDL_Surface	*s;
	int			w;
	int			h;
	bool		ok;

	if (SDL_GetRendererOutputSize(p->renderer, &w, &h) != 0)
		return (false);
	s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
	if (s == NULL)
		return (false);
	ok = SDL_RenderReadPixels(p->renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
			s->pixels, s->pitch) == 0 && SDL_SaveBMP(s, path) == 0;
	SDL_FreeSurface(s);
	return (ok);
}
