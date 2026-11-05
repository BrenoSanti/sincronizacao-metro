#include "sync_car.h"

void station_init(struct station *station)
{
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->seats_available, NULL);
    pthread_cond_init(&station->boarding_finished, NULL);
    station->waiting = 0;
    station->seats = 0;
    station->boarding = 0;
}

void station_load_car(struct station *station, int count)
{
    pthread_mutex_lock(&station->lock);
    station->seats = count > 0 ? count : 0;
    if (station->seats > 0 && station->waiting > 0)
        pthread_cond_broadcast(&station->seats_available);

    while (station->boarding > 0 || (station->seats > 0 && station->waiting > 0))
        pthread_cond_wait(&station->boarding_finished, &station->lock);

    /* As vagas que sobraram deixam de existir quando o carro parte. */
    station->seats = 0;
    pthread_mutex_unlock(&station->lock);
}

void station_wait_for_car(struct station *station)
{
    pthread_mutex_lock(&station->lock);
    station->waiting++;
    while (station->seats == 0)
        pthread_cond_wait(&station->seats_available, &station->lock);

    /* Reservar sob o lock impede que dois passageiros ocupem a última vaga. */
    station->waiting--;
    station->seats--;
    station->boarding++;
    pthread_mutex_unlock(&station->lock);
}

void station_on_board(struct station *station)
{
    pthread_mutex_lock(&station->lock);
    station->boarding--;
    if (station->boarding == 0)
        pthread_cond_signal(&station->boarding_finished);
    pthread_mutex_unlock(&station->lock);
}
