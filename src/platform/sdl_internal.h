#ifndef SDL_INTERNAL_H
# define SDL_INTERNAL_H

/*
** Private to src/platform/: the concrete platform object shared by the
** SDL video, input and (later) audio implementations.
*/

# include "platform.h"

# include <SDL.h>

# define MAX_TEXTURES 64

typedef struct s_audio	t_audio;

struct s_platform
{
	SDL_Window		*window;
	SDL_Renderer	*renderer;
	SDL_Texture		*textures[MAX_TEXTURES];
	SDL_Texture		*frame_tex;
	uint32_t		*frame;
	bool			img_ready;
	bool			vsync;
	bool			mouse_wanted;
	bool			mouse_captured;
	int				logical_w;
	int				logical_h;
	/* NULL when no audio device could be opened (the game stays silent). */
	t_audio			*audio;
};

void	platform_audio_close(t_platform *p);

#endif
