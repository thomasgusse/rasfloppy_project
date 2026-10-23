#include "floppy_driver.h"
#include "timer.h"
#include <pthread.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define DIR_PIN 23
#define STEP_PIN 24

unsigned long timer_us = 40;

unsigned int desired_period = 2000;
unsigned int time_since_low = 0;
char direction = 0;

int count = 0;

void floppy_handler(void) {
    if (count < 80) {
        if (time_since_low == 0) {
            digitalWrite(STEP_PIN, HIGH);
        }
        time_since_low += timer_us;
        if (time_since_low >= desired_period) {
            digitalWrite(STEP_PIN, LOW);
            time_since_low = 0;
            count++;
        }
    } else {
        direction = 1 - direction;
        digitalWrite(DIR_PIN, direction);
        count = 0;
    }
}

/* this function is the loop of the driver
 * it waits and does nothing
 * the main logic of moving the floppy driver head is performed by the floppy_handler function
 * which is called every 40 us
 */
void * driver_loop(void * _) {
    setup_timer(floppy_handler, timer_us);

    // wait and do nothing, the logic is done when the timer hits
    for(;;) delay(100);

    return NULL;
}
void init_drive(){
    wiringPiSetupPinType(WPI_PIN_BCM);

    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(STEP_PIN, LOW);
}

void reset_position(){
    digitalWrite(DIR_PIN,LOW);
    for (int i = 0; i < 100; ++i) {
        digitalWrite(STEP_PIN, HIGH);
        digitalWrite(STEP_PIN, LOW);
    }
    digitalWrite(DIR_PIN, HIGH);
    digitalWrite(STEP_PIN, HIGH);
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);


}

void init_driver() {
    // initializes WiringPi and setup the pins used as digital output
    // see https://github.com/WiringPi/WiringPi/blob/master/documentation/english/functions.md
    init_drive();

    // resets the head to a known position to avoid crashing it during play
    reset_position();
    
    // create a dedicated thread for the driver
    pthread_t driver_thread;
    pthread_create(&driver_thread, NULL, driver_loop, NULL);

    // the main thread ignores the signals sent by the timer
    ignore_signals();
}

void play_note(double frequency){
    int period = 1000000/frequency;
    desired_period = period;
}
