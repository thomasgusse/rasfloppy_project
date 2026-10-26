#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include "floppy_driver.h"



int main(void) {
    init_driver();
    printf("Floppy drive initialized\n");
    note frere_jacques_notes[] = {
        {262, 400}, {294, 400}, {330, 400}, {262, 400},
        {262, 400}, {294, 400}, {330, 400}, {262, 400},
        {330, 400}, {349, 400}, {392, 400}, {330, 400},
        {349, 400}, {392, 800}
    };
    music frere_jacques = {14, frere_jacques_notes};

    while (1) {
        for (int i = 0; i < frere_jacques.size_note; i++) {
            play_note(frere_jacques.notes[i].frequency);
            delay(frere_jacques.notes[i].duration);
        }
    }

    return 0;
}