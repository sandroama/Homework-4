#include <pthread.h>

struct station {
    pthread_mutex_t mutex;
    pthread_cond_t bus_arrived;
    pthread_cond_t student_boarded;
    pthread_cond_t all_boarded;
    int waiting_students;
    int free_seats;
    int boarded_students;
    int ticket_counter;
    int boarding_turn;
};

void
station_init(struct station *station)
{
    pthread_mutex_init(&station->mutex, NULL);
    pthread_cond_init(&station->bus_arrived, NULL);
    pthread_cond_init(&station->student_boarded, NULL);
    pthread_cond_init(&station->all_boarded, NULL);
    station->waiting_students = 0;
    station->free_seats = 0;
    station->boarded_students = 0;
    station->ticket_counter = 0;
    station->boarding_turn = 0;
}

void
station_load_bus(struct station *station, int count)
{
    void
station_load_bus(struct station *station, int count)
{
    pthread_mutex_lock(&station->mutex);
    station->free_seats = count;
    station->boarded_students = 0;

    int total_boarded_students = 0;

    while (station->waiting_students > 0 && station->free_seats > 0) {
        pthread_cond_signal(&station->bus_arrived);
        pthread_cond_wait(&station->student_boarded, &station->mutex);

        total_boarded_students += station->boarded_students;
        station->free_seats -= station->boarded_students;
        station->waiting_students -= station->boarded_students;
        station->boarded_students = 0;

        if (total_boarded_students >= count || station->waiting_students == 0) {
            break;
        }
    }

    station->free_seats = 0;
    pthread_cond_broadcast(&station->all_boarded);
    pthread_mutex_unlock(&station->mutex);
}

}


int
station_wait_for_bus(struct station *station, int myticket, int myid)
{
    pthread_mutex_lock(&station->mutex);
    station->waiting_students++;

    while (myticket != station->boarding_turn + 1 || station->free_seats <= 0) {
        pthread_cond_wait(&station->bus_arrived, &station->mutex);
    }

    station->boarding_turn++;
    station->waiting_students--;
    station->free_seats--;
    station->boarded_students++;

    printf("student %d with ticket %d has boarded, the turn is %d\n", myid, myticket, station->boarding_turn);

    pthread_cond_signal(&station->student_boarded);
    
    if (station->free_seats == 0 || station->waiting_students == 0) {
        pthread_cond_broadcast(&station->all_boarded); // added this line
    }
    
    pthread_mutex_unlock(&station->mutex);

    return station->boarding_turn;
}
