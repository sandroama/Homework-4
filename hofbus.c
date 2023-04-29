struct station {
    pthread_mutex_t lock;
    pthread_cond_t bus_arrived;
    pthread_cond_t student_boarded;
    int freeSeats;
    int waitStudents;
    int nextTicket;
    int counterTicket;
};

void station_init(struct station *station) {
  pthread_mutex_init(&station->lock, NULL);
  pthread_cond_init(&station->bus_arrived, NULL);
  pthread_cond_init(&station->student_boarded, NULL);
  station->freeSeats = 0;
  station->waitStudents = 0;
  station->nextTicket = 1;
  station->counterTicket = 1;
}

void station_load_bus(struct station *station, int count) {
  pthread_mutex_lock(&station->lock);

  if (count == 0 ||station->waitStudents == 0) {
    pthread_mutex_unlock(&station->lock);
    return;
  }

  station->freeSeats = count;

  pthread_cond_broadcast(&station->bus_arrived);

  while (station->waitStudents > 0 && station->freeSeats > 0) {
    pthread_cond_wait(&station->student_boarded, &station->lock);
  }

  station->freeSeats = 0;
  pthread_mutex_unlock(&station->lock);
}

int station_wait_for_bus(struct station *station, int myticket, int myid) {
  pthread_mutex_lock(&station->lock);
  station->waitStudents++;

  while (myticket != station->nextTicket || station->freeSeats == 0) {
    pthread_cond_wait(&station->bus_arrived, &station->lock);
  }

  station->waitStudents--;
  station->nextTicket++;
  station->freeSeats--;

  pthread_cond_signal(&station->student_boarded);
  pthread_mutex_unlock(&station->lock);

  return (myticket);
}