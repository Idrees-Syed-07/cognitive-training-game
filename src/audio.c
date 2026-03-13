#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

void audio_init(const char *filepath) {
    char cmd[512];
    mciSendStringA("close wall_sound", NULL, 0, NULL);
    snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias wall_sound", filepath);
    mciSendStringA(cmd, NULL, 0, NULL);
}

void audio_play(void) {
    mciSendStringA("play wall_sound from 0", NULL, 0, NULL);
}

void audio_free(void) {
    mciSendStringA("close wall_sound", NULL, 0, NULL);
}
