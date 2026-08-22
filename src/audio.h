#ifndef AUDIO_H_
#define AUDIO_H_

#include "common.h"

void audio_init(void);

void audio_play_tick(void);

void audio_play_alarm(void);

void audio_toggle_mute(void);

bool audio_is_muted(void);

void audio_free(void);

#endif
