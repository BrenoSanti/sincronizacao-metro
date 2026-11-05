#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sync_car.h"

/* O teste usa apenas a interface pública da estação. */
struct trip {
    struct station station;
    pthread_mutex_t records_lock;
    int seats;
    int expected;
    int reserved;
    int confirmed;
};

static void check_thread(int err)
{
    if (err != 0) {
        fprintf(stderr, "pthread: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
}

static void *run_car(void *arg)
{
    struct trip *trip = arg;
    const struct timespec interval = { .tv_nsec = 1000000 };
    for (;;) {
        station_load_car(&trip->station, trip->seats);
        pthread_mutex_lock(&trip->records_lock);
        if (trip->reserved > 0) {
            if (trip->confirmed != trip->expected) {
                fprintf(stderr, "Partida com %d de %d confirmações.\n",
                        trip->confirmed, trip->expected);
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&trip->records_lock);
            return NULL;
        }
        pthread_mutex_unlock(&trip->records_lock);
        /* O primeiro carro pode encontrar a estação vazia. Envie o próximo. */
        nanosleep(&interval, NULL);
    }
}

static void *reserve(void *arg)
{
    struct trip *trip = arg;
    station_wait_for_car(&trip->station);
    pthread_mutex_lock(&trip->records_lock);
    trip->reserved++;
    pthread_mutex_unlock(&trip->records_lock);
    return NULL;
}

static void admit_one(struct trip *trip)
{
    pthread_t passenger;
    check_thread(pthread_create(&passenger, NULL, reserve, trip));
    check_thread(pthread_join(passenger, NULL));
}

static void confirm_one(struct trip *trip)
{
    pthread_mutex_lock(&trip->records_lock);
    station_on_board(&trip->station);
    trip->confirmed++;
    pthread_mutex_unlock(&trip->records_lock);
}

int main(void)
{
    struct trip a = { .records_lock = PTHREAD_MUTEX_INITIALIZER, .seats = 3, .expected = 2 };
    struct trip b = { .records_lock = PTHREAD_MUTEX_INITIALIZER, .seats = 1, .expected = 1 };
    pthread_t car_a, car_b, first_passenger;

    alarm(10);
    station_init(&a.station);
    station_init(&b.station);

    check_thread(pthread_create(&first_passenger, NULL, reserve, &a));
    station_load_car(&a.station, -1);
    pthread_mutex_lock(&a.records_lock);
    if (a.reserved != 0) {
        fputs("Uma capacidade negativa liberou um assento.\n", stderr);
        return EXIT_FAILURE;
    }
    pthread_mutex_unlock(&a.records_lock);
    puts("OK: capacidade -1 retorna sem conceder uma vaga.");

    check_thread(pthread_create(&car_a, NULL, run_car, &a));
    check_thread(pthread_join(first_passenger, NULL));
    admit_one(&a);  /* Duas reservas antes da primeira confirmação. */
    confirm_one(&a);

    check_thread(pthread_create(&car_b, NULL, run_car, &b));
    admit_one(&b);
    confirm_one(&b);
    check_thread(pthread_join(car_b, NULL));

    confirm_one(&a);  /* A só parte agora, com uma vaga sobrando. */
    check_thread(pthread_join(car_a, NULL));
    alarm(0);
    puts("OK: duas reservas simultâneas e duas estações independentes.");
    return 0;
}
