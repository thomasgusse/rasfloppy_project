CC      = gcc
CFLAGS  = -Wall -Wextra
TARGET  = rasfloppy
OBJS    = main.o floppy_driver.o timer.o      # timer.o est fourni (pas de timer.c)

SIM_SRC = main.c floppy_driver.c sim/wiringpi_mock.c sim/timer_mock.c

# sans ceci, la première cible du fichier deviendrait la cible par défaut
.DEFAULT_GOAL := all
.PHONY: all sim clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -pthread -lwiringPi -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c floppy_driver.h
floppy_driver.o: floppy_driver.c floppy_driver.h timer.h

# simulation sur PC (sans Raspberry Pi) : make sim && ./rasfloppy_sim
sim: rasfloppy_sim
rasfloppy_sim: $(SIM_SRC) floppy_driver.h timer.h sim/wiringPi.h
	$(CC) $(CFLAGS) -I. -Isim $(SIM_SRC) -pthread -lm -o $@

# ne supprime PAS timer.o 
clean:
	rm -f main.o floppy_driver.o $(TARGET) rasfloppy_sim steps.log sortie.wav