#pragma once

void floppy_handler();

void init_driver();
void play_note(double frequency);
void init_drive();
void reset_position();

typedef struct {
    double frequency;
    unsigned int duration;
} note;

typedef struct {
    int size_note;
    note *notes;
} music;