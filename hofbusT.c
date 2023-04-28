#include <pthread.h>

struct station {
    pthread_mutex_t mutex;                  // Mutex to protect critical sections
    pthread_cond_t bus_arrived;             // Condition variable to signal students when the bus arrives
    pthread_cond_t student_boarded;         // Condition variable to signal when a student has boarded
    pthread_cond_t all_boarded;             // Condition variable to signal when all students have boarded
    int waiting_students;                   // Number of students waiting at the station
    int free_seats;                         // Number of free seats available on the bus
    int boarded_students;                   // Number of students who have boarded the bus
    int ticket_counter;                     // Ticket counter to keep track of the student's ticket number
    int boarding_turn;                      // Turn number for boarding students
};

void
station_init(struct station *station)
{
    pthread_mutex_init(&station->mutex, NULL);             // Initialize the mutex
    pthread_cond_init(&station->bus_arrived, NULL);        // Initialize the bus_arrived condition variable
    pthread_cond_init(&station->student_boarded, NULL);    // Initialize the student_boarded condition variable
    pthread_cond_init(&station->all_boarded, NULL);        // Initialize the all_boarded condition variable
    station->waiting_students = 0;                         // Initialize the waiting_students counter
    station->free_seats = 0;                               // Initialize the free_seats counter
    station->boarded_students = 0;                         // Initialize the boarded_students counter
    station->ticket_counter = 0;                           // Initialize the ticket_counter
    station->boarding_turn = 0;                            // Initialize the boarding_turn counter
}

void
station_load_bus(struct station *station, int count)
{
    pthread_mutex_lock(&station->mutex);                   // Lock the mutex
    station->free_seats = count;                           // Set the free_seats to the number of seats available on the bus
    station->boarded_students = 0;                         // Reset the boarded_students counter

    while (station->free_seats > 0 && station->waiting_students > 0) {
        pthread_cond_signal(&station->bus_arrived);        // Signal waiting students that the bus has arrived
        pthread_cond_wait(&station->student_boarded, &station->mutex); // Wait for a student to board
    }

    station->free_seats = 0;                               // Reset the free_seats counter
    pthread_cond_broadcast(&station->all_boarded);         // Signal all students that the bus is full or everyone has boarded
    pthread_mutex_unlock(&station->mutex);                 // Unlock the mutex
}

int
station_wait_for_bus(struct station *station, int myticket, int myid)
{
    pthread_mutex_lock(&station->mutex);                   // Lock the mutex
    station->waiting_students++;                           // Increment the waiting_students counter

    // Wait for the bus to arrive and for it to be the student's turn to board
    while (myticket != station->boarding_turn + 1 || station->free_seats <= 0) {
        pthread_cond_wait(&station->bus_arrived, &station->mutex); // Wait for the bus to arrive
    }

    station->boarding_turn++;                              // Increment the boarding_turn counter
    station->waiting_students--;                           // Decrement the waiting_students counter
    station->free_seats--;                                 // Decrement the free_seats counter
    station->boarded_students++;                           // Increment the boarded_students counter

    pthread_cond_signal(&station->student_boarded);        // Signal that the student has boarded

    pthread_mutex_unlock(&station->mutex);                 // Unlock the mutex

    return station->boarding_turn;                         // Return the student's boarding turn
}


// int
// station_wait_for_bus(struct station *station, int myticket, int myid)
// {
//     pthread_mutex_lock(&station->mutex);                   // Lock the mutex
//     station->waiting_students++;                           // Increment the waiting_students counter

//     // Wait for the bus to arrive and for it to be the student's turn to board
//     while (myticket != station->boarding_turn + 1 || station->free_seats <= 0) {
//         pthread_cond_wait(&station->bus_arrived, &station->mutex); // Wait for the bus to arrive
//     }

//     station->boarding_turn++;                              // Increment the boarding_turn counter
//     station->waiting_students--;                           // Decrement the waiting_students counter

//     pthread_mutex_unlock(&station->mutex);                 // Unlock the mutex

//     return station->boarding_turn;                         // Return the student's boarding turn
// }
