#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

static int  audio_initialized = 0; 

void audio_init(constant char *filepath){

    char cmdbuf[512];

    mciSendStringA("close wall sound", NULL, 0, NULL);
   snprintf(cmdbuff, sizeof(cmdbuff), "open \"%s\" type mpegvideo alias wall_sound", filepath);
    if (mciSendStringA(cmdbuff, NULL, 0, NULL)  != 0) fprintf(stderr, "audio_init failed\n");
    audio_initialized = 1;

}

void audioPlay(void){
    if (audio_intialized ==0) return;
    mciSendStringA("play wall_sound from 0", NULL, 0, NULL);

}

void audio_free(void){
    mciSendStringA("close wall_sound", NULL, 0, NULL);

}