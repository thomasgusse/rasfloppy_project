#pragma once

void ignore_signals();
int setup_timer(void (*callback)(void), unsigned int interval_us);