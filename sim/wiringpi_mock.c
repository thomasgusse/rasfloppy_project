#define _POSIX_C_SOURCE 200809L
#include "wiringPi.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIM_DIR_PIN  23     /* mêmes numéros BCM que floppy_driver.c */
#define SIM_STEP_PIN 24
#define MAX_EVENTS   1000000

typedef struct { long long t_us; int dir; } Step;
typedef struct { long long t_us; char label[32]; } Mark;

static Step steps[MAX_EVENTS];
static Mark marks[64];
static volatile int nb_steps = 0, nb_marks = 0;
static int pin_state[64];

static long long now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}

static void dump_events(void) {
    static int done = 0;
    if (done) return;
    done = 1;
    FILE *f = fopen("steps.log", "w");
    if (!f) return;
    for (int i = 0; i < nb_marks; i++)
        fprintf(f, "# %s %lld\n", marks[i].label, marks[i].t_us);
    for (int i = 0; i < nb_steps; i++)
        fprintf(f, "%lld %d\n", steps[i].t_us, steps[i].dir);
    fclose(f);
    fprintf(stderr, "[sim] %d pas enregistrés dans steps.log\n", nb_steps);
}

static void on_signal(int sig) { (void)sig; dump_events(); _exit(0); }

int wiringPiSetupPinType(enum WPIPinType type) {
    (void)type;
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    atexit(dump_events);
    return 0;
}

void pinMode(int pin, int mode) { (void)pin; (void)mode; }

void digitalWrite(int pin, int value) {
    if (pin < 0 || pin >= 64) return;
    /* d'après l'énoncé, un pas a lieu quand STEP passe de 5 V à GND */
    if (pin == SIM_STEP_PIN && pin_state[pin] && !value && nb_steps < MAX_EVENTS) {
        steps[nb_steps].t_us = now_us();
        steps[nb_steps].dir  = pin_state[SIM_DIR_PIN];
        nb_steps++;
    }
    pin_state[pin] = value;
}

void sim_mark(const char *label) {
    if (nb_marks < 64) {
        marks[nb_marks].t_us = now_us();
        strncpy(marks[nb_marks].label, label, 31);
        marks[nb_marks].label[31] = 0;
        nb_marks++;
    }
}

void delay(unsigned int ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

void delayMicroseconds(unsigned int us) {
    struct timespec ts = { us / 1000000, (us % 1000000) * 1000L };
    nanosleep(&ts, NULL);
}
