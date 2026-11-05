#ifndef SYNC_CAR_H
#define SYNC_CAR_H

#include <pthread.h>

struct station {
    pthread_mutex_t lock;
    pthread_cond_t seats_available;
    pthread_cond_t boarding_finished;
    int waiting;    /* Passageiros que ainda não reservaram um assento. */
    int seats;      /* Assentos disponíveis para reserva no carro atual. */
    int boarding;   /* Reservas ainda sem confirmação de embarque. */
};

void station_init(struct station *station);
void station_load_car(struct station *station, int count);
void station_wait_for_car(struct station *station);
void station_on_board(struct station *station);

#endif
