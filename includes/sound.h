#ifndef SOUND_H
# define SOUND_H

# include "game.h"
# include "platform.h"

typedef enum e_sfx
{
	SND_STEP,
	SND_HEARTBEAT,
	SND_KEY,
	SND_BATTERY,
	SND_LOCKED,
	SND_DOOR,
	SND_SPOTTED,
	SND_GROWL,
	SND_SCREAM,
	SND_LIGHT,
	SND_LIGHT_DEAD,
	SND_BREATH,
	SND_PAUSE,
	SND_JINGLE_ESCAPE,
	SND_JINGLE_OVER,
	SND_JINGLE_VICTORY,
	SND_MUSIC_GAME,
	SND_MUSIC_TITLE,
	SND_COUNT
}	t_sfx;

typedef struct s_sound_bank
{
	t_sound	sounds[SND_COUNT];
	int		loaded;
}	t_sound_bank;

/* Opens the audio device and loads every clip (missing ones stay silent). */
void	sound_init(t_sound_bank *b, t_platform *p);
/* Plays this frame's events and keeps the right loop running. */
void	sound_update(const t_sound_bank *b, t_platform *p, const t_game *g);

#endif
