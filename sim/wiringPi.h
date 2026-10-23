/* Faux wiringPi.h pour tester le projet sur un PC (aucun GPIO réel). */
#ifndef WIRINGPI_MOCK_H
#define WIRINGPI_MOCK_H

#define INPUT  0
#define OUTPUT 1
#define LOW    0
#define HIGH   1

enum WPIPinType { WPI_PIN_BCM, WPI_PIN_WPI, WPI_PIN_PHYS };

int  wiringPiSetupPinType(enum WPIPinType type);
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
void delay(unsigned int ms);
void delayMicroseconds(unsigned int us);

/* spécifique à la simulation */
void sim_mark(const char *label);

#endif
