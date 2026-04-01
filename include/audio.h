// AUDIO ONLY WORKS ON WINDOWS //

#ifndef AUDIO_H
#define AUDIO_H

//this function loads the audio file into memory to make playback faster
void load_audio(char *filename);
//this function playes the loaded audio file
void play_audio();
//this function frees the audio file from memory
void free_audio();

#endif
