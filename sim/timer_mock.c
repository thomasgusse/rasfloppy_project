#define _POSIX_C_SOURCE 200809L
#include "timer.h"
#include "wiringPi.h"
#include <pthread.h>
#include <time.h>

static void (*cb)(void);
static unsigned int interval_us;

static void *timer_thread(void *arg) {
    (void)arg;
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    for (;;) {
        next.tv_nsec += interval_us * 1000L;
        while (next.tv_nsec >= 1000000000L) {
            next.tv_nsec -= 1000000000L;
            next.tv_sec++;
        }
        /* temps absolu : si on prend du retard, on rattrape (la fréquence moyenne reste juste) */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        cb();
    }
    return NULL;
}

int setup_timer(void (*callback)(void), unsigned int interval) {
    pthread_t t;
    cb = callback;
    interval_us = interval;
    sim_mark("timer_start");
    return pthread_create(&t, NULL, timer_thread, NULL);
}

void ignore_signals() { /* pas de signaux dans la simulation */ }
