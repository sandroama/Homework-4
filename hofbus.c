#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Struct to store station information
typedef struct {
    pthread_mutex_t lock;               // Mutex lock to protect critical sections
    pthread_cond_t busArrived;          // Condition variable to signal when a bus arrives
    pthread_cond_t passengerBoarded;    // Condition variable to signal when a passenger has boarded the bus
    int availableSeats;                 // Number of available seats on the bus
    int waitingPassengers;              // Number of passengers waiting at the station
    int nextTicket;                     // Ticket number for the next passenger to board
    int ticketCounter;                  // Ticket counter for assigning ticket numbers to new passengers

} Station;

// Initialize station object
void stationInit(Station *station) {
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->busArrived, NULL);
    pthread_cond_init(&station->passengerBoarded, NULL);
    station->availableSeats = 0;
    station->waitingPassengers = 0;
    station->nextTicket = 1;
    station->ticketCounter = 1;
}

// Load bus with passengers
void stationLoadBus(Station *station, int seatCount) {
    pthread_mutex_lock(&station->lock);                       // Lock the mutex

    // If there are no waiting passengers or seats available, return
    if (station->waitingPassengers == 0 || seatCount == 0) {
        pthread_mutex_unlock(&station->lock);
        return;
    }

    station->availableSeats = seatCount;                      // Set the number of available seats on the bus
    pthread_cond_broadcast(&station->busArrived);             // Signal all waiting passengers that the bus has arrived

    // Wait until all available seats are filled or no waiting passengers remain
    while (station->availableSeats > 0 && station->waitingPassengers > 0) {
        pthread_cond_wait(&station->passengerBoarded, &station->lock); // Wait for a passenger to board the bus
    }

    station->availableSeats = 0;                              // Reset the number of available seats to 0
    pthread_mutex_unlock(&station->lock);                     // Unlock the mutex
}

// Passenger waits for the bus and returns their ticket number
int stationWaitForBus(Station *station, int passengerTicket, int passengerId) {
    pthread_mutex_lock(&station->lock);                       // Lock the mutex
    station->waitingPassengers++;                             // Increment the number of waiting passengers

    // Wait until it's the passenger's turn and there are available seats
    while (passengerTicket != station->nextTicket || station->availableSeats == 0) {
        pthread_cond_wait(&station->busArrived, &station->lock); // Wait for the bus to arrive
    }

    station->waitingPassengers--;                             // Decrement the number of waiting passengers
    station->nextTicket++;                                    // Increment the nextTicket counter
    station->availableSeats--;                                // Decrement the number of available seats

    pthread_cond_signal(&station->passengerBoarded);          // Signal that the passenger has boarded the bus
    pthread_mutex_unlock(&station->lock);                     // Unlock the mutex

    returnpassengerTicket;
}
