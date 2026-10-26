#pragma once

void floppy_handler();

void init_driver();
void play_note(double frequency);
void init_drive();
void reset_position();

// define a music by the number of notes which are the frequency and the duration of the note

typedef struct {
    double frequency;
    unsigned int duration;
} note;

typedef struct {  
    int size_note;
    note *notes;
} music;