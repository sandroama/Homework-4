#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t busArrived;
    pthread_cond_t passengerBoarded;
    int availableSeats;
    int waitingPassengers;
    int nextTicket;
    int ticketCounter;
  
} Station;

void stationInit(Station *station) {
pthread_mutex_init(&station->lock, NULL);
  pthread_cond_init(&station->busArrived, NULL);
  pthread_cond_init(&station->passengerBoarded, NULL);
  station->availableSeats = 0;
  station->waitingPassengers = 0;
  station->nextTicket = 1;
  station->ticketCounter = 1;

}

void stationLoadBus(Station *station, int count) {
  pthread_mutex_lock(&station->lock);

  if (station->waitingPassengers == 0 || count == 0) {
    pthread_mutex_unlock(&station->lock);
    return;
  }

  station->availableSeats = count;
  pthread_cond_broadcast(&station->busArrived);

  while (station->availableSeats > 0 && station->waitingPassengers > 0) {
    pthread_cond_wait(&station->passengerBoarded, &station->lock);
  }

  station->availableSeats = 0;
  pthread_mutex_unlock(&station->lock);
}

int stationWaitForBus(Station *station, int myTicket, int myId) {
  pthread_mutex_lock(&station->lock);
  station->waitingPassengers++;

  while (myTicket != station->nextTicket || station->availableSeats == 0) {
    pthread_cond_wait(&station->busArrived, &station->lock);
  }

  station->waitingPassengers--;
  station->nextTicket++;
  station->availableSeats--;

  pthread_cond_signal(&station->passengerBoarded);
  pthread_mutex_unlock(&station->lock);

  return myTicket;
}
