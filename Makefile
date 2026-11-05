CC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu11
THREAD_FLAGS = -pthread

.PHONY: all test clean

all: sync_car

sync_car: sync_car.o sync_car-runner.o
	$(CC) $(LDFLAGS) $(THREAD_FLAGS) -o $@ $^ $(LDLIBS)

test_stations: sync_car.o test_stations.o
	$(CC) $(LDFLAGS) $(THREAD_FLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c sync_car.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THREAD_FLAGS) -c $< -o $@

test: sync_car test_stations
	./sync_car
	./test_stations

clean:
	$(RM) sync_car test_stations sync_car.o sync_car-runner.o test_stations.o
