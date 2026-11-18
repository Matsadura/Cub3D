#include "sdl_internal.h"

#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#endif

/*
** Minimal software mixer on top of the SDL2 audio callback, so the game
** needs no extra library (no SDL_mixer) natively or in the browser.
** All clips are converted once at load time to the device format:
** 22050 Hz, signed 16-bit, mono.
*/

#define AUDIO_RATE      22050
#define AUDIO_FRAMES    1024
#define MAX_SOUNDS      32
#define MAX_VOICES      8
#define VOLUME_ONE      256

typedef struct s_clip
{
	Sint16	*samples;
	Uint32	count;
}	t_clip;

typedef struct s_voice
{
	const t_clip	*clip;
	Uint32			pos;
	int				volume;
	Uint32			age;
}	t_voice;

struct s_audio
{
	SDL_AudioDeviceID	dev;
	t_clip				clips[MAX_SOUNDS];
	t_voice				voices[MAX_VOICES];
	t_voice				music;
	t_sound				music_id;
	bool				music_paused;
	bool				muted;
	Uint32				clock;
};

#ifdef __EMSCRIPTEN__

EM_JS(int, js_load_muted, (void), {
	try {
		return window.localStorage.getItem("darkmaze.muted") === "1" ? 1 : 0;
	} catch (e) {
		return 0;
	}
});

EM_JS(void, js_save_muted, (int muted), {
	try {
		window.localStorage.setItem("darkmaze.muted", muted ? "1" : "0");
	} catch (e) {}
});
#endif

bool	platform_load_muted(void)
{
#ifdef __EMSCRIPTEN__
	return (js_load_muted() != 0);
#else
	return (false);
#endif
}

void	platform_save_muted(bool muted)
{
#ifdef __EMSCRIPTEN__
	js_save_muted(muted);
#else
	(void)muted;
#endif
}

static Sint32	mix_voice(t_voice *v, bool loop)
{
	Sint32	s;

	if (v->clip == NULL || v->clip->count == 0)
		return (0);
	s = (Sint32)v->clip->samples[v->pos] * v->volume / VOLUME_ONE;
	if (++v->pos >= v->clip->count)
	{
		v->pos = 0;
		if (!loop)
			v->clip = NULL;
	}
	return (s);
}

/* Runs on the audio thread (natively) or the Web Audio callback. */
static void	mix(void *userdata, Uint8 *stream, int len)
{
	t_audio	*a;
	Sint16	*out;
	Sint32	acc;
	int		i;
	int		v;

	a = userdata;
	out = (Sint16 *)stream;
	i = 0;
	while (i < len / (int)sizeof(Sint16))
	{
		acc = 0;
		v = 0;
		while (v < MAX_VOICES)
			acc += mix_voice(&a->voices[v++], false);
		if (!a->music_paused)
			acc += mix_voice(&a->music, true);
		if (acc > 32767)
			acc = 32767;
		if (acc < -32768)
			acc = -32768;
		out[i++] = a->muted ? 0 : (Sint16)acc;
	}
}

bool	platform_audio_open(t_platform *p)
{
	SDL_AudioSpec	want;
	t_audio			*a;

	if (p->audio != NULL)
		return (true);
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
		return (false);
	a = calloc(1, sizeof(*a));
	if (a == NULL)
	{
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return (false);
	}
	SDL_zero(want);
	want.freq = AUDIO_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = AUDIO_FRAMES;
	want.callback = mix;
	want.userdata = a;
	a->music_id = SOUND_NONE;
	a->dev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
	if (a->dev == 0)
	{
		free(a);
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return (false);
	}
	p->audio = a;
	SDL_PauseAudioDevice(a->dev, 0);
	return (true);
}

void	platform_audio_close(t_platform *p)
{
	t_audio	*a;
	int		i;

	a = p->audio;
	if (a == NULL)
		return ;
	SDL_CloseAudioDevice(a->dev);
	i = 0;
	while (i < MAX_SOUNDS)
		free(a->clips[i++].samples);
	free(a);
	p->audio = NULL;
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/* Converts a loaded WAV to the mixer format (22050 Hz s16 mono). */
static bool	convert_clip(t_clip *clip, SDL_AudioSpec *spec, Uint8 *buf,
		Uint32 len)
{
	SDL_AudioCVT	cvt;

	if (SDL_BuildAudioCVT(&cvt, spec->format, spec->channels, spec->freq,
			AUDIO_S16SYS, 1, AUDIO_RATE) < 0)
		return (false);
	cvt.len = (int)len;
	cvt.buf = malloc((size_t)len * (cvt.len_mult > 0 ? cvt.len_mult : 1));
	if (cvt.buf == NULL)
		return (false);
	memcpy(cvt.buf, buf, len);
	if (cvt.needed && SDL_ConvertAudio(&cvt) != 0)
	{
		free(cvt.buf);
		return (false);
	}
	clip->samples = (Sint16 *)cvt.buf;
	clip->count = (Uint32)(cvt.needed ? cvt.len_cvt : cvt.len)
		/ sizeof(Sint16);
	return (true);
}

t_sound	platform_load_sound(t_platform *p, const char *path)
{
	SDL_AudioSpec	spec;
	Uint8			*buf;
	Uint32			len;
	int				i;
	bool			ok;

	if (p->audio == NULL)
		return (SOUND_NONE);
	i = 0;
	while (i < MAX_SOUNDS && p->audio->clips[i].samples != NULL)
		i++;
	if (i == MAX_SOUNDS)
		return (SOUND_NONE);
	if (SDL_LoadWAV(path, &spec, &buf, &len) == NULL)
		return (SOUND_NONE);
	ok = convert_clip(&p->audio->clips[i], &spec, buf, len);
	SDL_FreeWAV(buf);
	return (ok ? i : SOUND_NONE);
}

static int	to_volume(float volume)
{
	if (volume < 0.0f)
		volume = 0.0f;
	if (volume > 1.0f)
		volume = 1.0f;
	return ((int)(volume * VOLUME_ONE));
}

static bool	valid(const t_platform *p, t_sound s)
{
	return (p->audio != NULL && s >= 0 && s < MAX_SOUNDS
		&& p->audio->clips[s].samples != NULL);
}

/* Uses a free voice, or steals the oldest one. */
void	platform_play_sound(t_platform *p, t_sound s, float volume)
{
	t_audio	*a;
	int		pick;
	int		i;

	if (!valid(p, s))
		return ;
	a = p->audio;
	SDL_LockAudioDevice(a->dev);
	pick = 0;
	i = 0;
	while (i < MAX_VOICES)
	{
		if (a->voices[i].clip == NULL)
		{
			pick = i;
			break ;
		}
		if (a->voices[i].age < a->voices[pick].age)
			pick = i;
		i++;
	}
	a->voices[pick] = (t_voice){&a->clips[s], 0, to_volume(volume),
		++a->clock};
	SDL_UnlockAudioDevice(a->dev);
}

void	platform_play_music(t_platform *p, t_sound s, float volume)
{
	t_audio	*a;

	if (!valid(p, s))
		return ;
	a = p->audio;
	SDL_LockAudioDevice(a->dev);
	if (a->music_id != s || a->music.clip == NULL)
	{
		a->music = (t_voice){&a->clips[s], 0, to_volume(volume), 0};
		a->music_id = s;
	}
	a->music_paused = false;
	SDL_UnlockAudioDevice(a->dev);
}

void	platform_stop_music(t_platform *p)
{
	if (p->audio == NULL)
		return ;
	SDL_LockAudioDevice(p->audio->dev);
	p->audio->music.clip = NULL;
	p->audio->music_id = SOUND_NONE;
	SDL_UnlockAudioDevice(p->audio->dev);
}

void	platform_pause_music(t_platform *p, bool paused)
{
	if (p->audio == NULL)
		return ;
	SDL_LockAudioDevice(p->audio->dev);
	p->audio->music_paused = paused;
	SDL_UnlockAudioDevice(p->audio->dev);
}

void	platform_set_muted(t_platform *p, bool muted)
{
	if (p->audio == NULL)
		return ;
	SDL_LockAudioDevice(p->audio->dev);
	p->audio->muted = muted;
	SDL_UnlockAudioDevice(p->audio->dev);
}

bool	platform_is_muted(const t_platform *p)
{
	return (p->audio != NULL && p->audio->muted);
}
