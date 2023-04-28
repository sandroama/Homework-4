#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct station {
    int ticket_turn;
    int students_boarded;
    int waiting_students;
    int bus_seats;
    pthread_mutex_t lock;
    pthread_cond_t boarding_allowed;
    pthread_cond_t bus_depart;
};

void
station_init(struct station *station)
{
    station->ticket_turn = 1;
    station->students_boarded = 0;
    station->waiting_students = 0;
    station->bus_seats = 0;
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->boarding_allowed, NULL);
    pthread_cond_init(&station->bus_depart, NULL);
}

void
station_load_bus(struct station *station, int count)
{
    pthread_mutex_lock(&station->lock);
    printf("Bus entering station with %d free seats\n", count);
    station->bus_seats = count;

    int boarding_students = (station->waiting_students < count) ? station->waiting_students : count;
    for (int i = 0; i < boarding_students; i++) {
        pthread_cond_broadcast(&station->boarding_allowed);
    }

    pthread_cond_wait(&station->bus_depart, &station->lock);
    station->bus_seats = 0;
    pthread_mutex_unlock(&station->lock);
}

int
station_wait_for_bus(struct station *station, int myticket, int myid)
{
    pthread_mutex_lock(&station->lock);
    station->waiting_students++;
    printf("Student %d got ticket %d and start waiting\n", myid, myticket);

    while (myticket != station->students_boarded + 1 || station->bus_seats == 0) {
        pthread_cond_wait(&station->boarding_allowed, &station->lock);
    }

    printf("Student %d with ticket %d has boarded, the turn is %d\n", myid, myticket, station->students_boarded + 1);
    station->students_boarded++;
    station->waiting_students--;
    station->bus_seats--;

    if (station->bus_seats == 0 || station->students_boarded % m == 0) {
        pthread_cond_signal(&station->bus_depart);
    }

    pthread_mutex_unlock(&station->lock);
    return 1;
}
