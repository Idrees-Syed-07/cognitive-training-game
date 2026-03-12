#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

void audio_play(const char *filepath) {
    char cmd[512];
    /* Stop any currently playing sound, then play the new one */
    mciSendStringA("close wall_sound", NULL, 0, NULL);
    snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias wall_sound", filepath);
    mciSendStringA(cmd, NULL, 0, NULL);
    mciSendStringA("play wall_sound from 0", NULL, 0, NULL);
}
