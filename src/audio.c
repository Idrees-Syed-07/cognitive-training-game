// AUDIO ONLY WORKS ON WINDOWS //

#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

static char *sound_buffer = NULL; 

//loading audio into memory for fastery playback
void load_audio(char *filename) {
    FILE *f = fopen(filename, "rb"); //opening file
    fseek(f, 0, SEEK_END); //go to end of file
    long size = ftell(f); //getting size of file
    rewind(f); //go back to beginning of file

    sound_buffer = malloc(size);

    fread(sound_buffer, 1, size, f); //reading file into buffer

    fclose(f); //closing file

    //playing empty sound to start audio player and avoid delay on first play
    PlaySound(sound_buffer, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    PlaySound(NULL, NULL, 0);
}

void play_audio() {
    PlaySound(sound_buffer, NULL, SND_MEMORY | SND_ASYNC);
}

void free_audio() {
    free(sound_buffer);
}