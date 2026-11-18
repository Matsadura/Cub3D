#include "sound.h"

#include <stdio.h>

#define VOL_MUSIC	0.55f

static const char	*g_sound_paths[SND_COUNT] = {
[SND_STEP] = "assets/sounds/sfx_step.wav",
[SND_HEARTBEAT] = "assets/sounds/sfx_heartbeat.wav",
[SND_KEY] = "assets/sounds/sfx_key.wav",
[SND_BATTERY] = "assets/sounds/sfx_battery.wav",
[SND_LOCKED] = "assets/sounds/sfx_locked.wav",
[SND_DOOR] = "assets/sounds/sfx_door.wav",
[SND_SPOTTED] = "assets/sounds/sfx_spotted.wav",
[SND_GROWL] = "assets/sounds/sfx_growl.wav",
[SND_SCREAM] = "assets/sounds/sfx_scream.wav",
[SND_LIGHT] = "assets/sounds/sfx_light.wav",
[SND_LIGHT_DEAD] = "assets/sounds/sfx_light_dead.wav",
[SND_BREATH] = "assets/sounds/sfx_breath.wav",
[SND_PAUSE] = "assets/sounds/sfx_pause.wav",
[SND_JINGLE_ESCAPE] = "assets/sounds/jingle_escape.wav",
[SND_JINGLE_OVER] = "assets/sounds/jingle_game_over.wav",
[SND_JINGLE_VICTORY] = "assets/sounds/jingle_victory.wav",
[SND_MUSIC_GAME] = "assets/sounds/amb_drone.wav",
[SND_MUSIC_TITLE] = "assets/sounds/music_title.wav",
};

void	sound_init(t_sound_bank *b, t_platform *p)
{
	bool	audio;
	int		i;

	audio = platform_audio_open(p);
	if (audio)
		platform_set_muted(p, platform_load_muted());
	b->loaded = 0;
	i = -1;
	while (++i < SND_COUNT)
	{
		b->sounds[i] = SOUND_NONE;
		if (!audio)
			continue ;
		b->sounds[i] = platform_load_sound(p, g_sound_paths[i]);
		if (b->sounds[i] == SOUND_NONE)
			fprintf(stderr, "Warning: cannot load %s: %s\n",
				g_sound_paths[i], platform_error());
		else
			b->loaded++;
	}
}

static void	play(const t_sound_bank *b, t_platform *p, t_sfx s, float vol)
{
	platform_play_sound(p, b->sounds[s], vol);
}

/* Closer stalkers sound louder (danger is 0..1 by distance). */
static void	play_events(const t_sound_bank *b, t_platform *p, const t_game *g)
{
	uint32_t	ev;
	float		near;

	ev = g->events;
	near = 0.3f + 0.7f * (float)g->danger;
	if (ev & EV_STEP)
		play(b, p, SND_STEP, g->sprinting ? 0.55f : 0.3f);
	if (ev & EV_HEARTBEAT)
		play(b, p, SND_HEARTBEAT, near);
	if (ev & EV_GROWL)
		play(b, p, SND_GROWL, near);
	if (ev & EV_KEY)
		play(b, p, SND_KEY, 0.7f);
	if (ev & EV_BATTERY)
		play(b, p, SND_BATTERY, 0.6f);
	if (ev & EV_LOCKED)
		play(b, p, SND_LOCKED, 0.6f);
	if (ev & EV_SPOTTED)
		play(b, p, SND_SPOTTED, 0.75f);
	if (ev & EV_CAUGHT)
		play(b, p, SND_SCREAM, 1.0f);
	if (ev & EV_LIGHT)
		play(b, p, SND_LIGHT, 0.5f);
	if (ev & EV_LIGHT_DEAD)
		play(b, p, SND_LIGHT_DEAD, 0.6f);
	if (ev & EV_EXHAUSTED)
		play(b, p, SND_BREATH, 0.6f);
	if (ev & EV_PAUSE)
		play(b, p, SND_PAUSE, 0.5f);
}

static void	play_jingles(const t_sound_bank *b, t_platform *p, const t_game *g)
{
	if (g->events & EV_ESCAPE)
	{
		play(b, p, SND_DOOR, 0.8f);
		play(b, p, SND_JINGLE_ESCAPE, 0.6f);
	}
	if (g->events & EV_GAME_OVER)
		play(b, p, SND_JINGLE_OVER, 0.8f);
	if (g->events & EV_VICTORY)
		play(b, p, SND_JINGLE_VICTORY, 0.8f);
}

/* Title theme on the title screen, the maze drone while playing, silence
** under the scream and the jingles. */
static void	update_music(const t_sound_bank *b, t_platform *p, const t_game *g)
{
	if (g->state == GAME_TITLE)
		platform_play_music(p, b->sounds[SND_MUSIC_TITLE], VOL_MUSIC);
	else if (g->state == GAME_PLAYING)
		platform_play_music(p, b->sounds[SND_MUSIC_GAME], VOL_MUSIC);
	else if (g->state == GAME_PAUSED)
		platform_pause_music(p, true);
	else
		platform_stop_music(p);
}

void	sound_update(const t_sound_bank *b, t_platform *p, const t_game *g)
{
	play_events(b, p, g);
	play_jingles(b, p, g);
	update_music(b, p, g);
}
