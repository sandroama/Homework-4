struct station {
  pthread_mutex_t mutex;
  pthread_cond_t bus_arrived;
  pthread_cond_t student_boarded;
  int waiting_students;
  int free_seats;
  int ticket_counter;
  int boarding_turn;
};

void
station_init(struct station *station)
{
  pthread_mutex_init(&station->mutex, NULL);
  pthread_cond_init(&station->bus_arrived, NULL);
  pthread_cond_init(&station->student_boarded, NULL);
  station->waiting_students = 0;
  station->free_seats = 0;
  station->ticket_counter = 0;
  station->boarding_turn = 0;
}

void
station_load_bus(struct station *station, int count)
{
  pthread_mutex_lock(&station->mutex);
  station->free_seats = count;

  // Broadcast to all waiting students that a bus has arrived
  pthread_cond_broadcast(&station->bus_arrived);

  // Wait for all students to board the bus
  while (station->free_seats > 0 && station->waiting_students > 0) {
    pthread_cond_wait(&station->student_boarded, &station->mutex);
  }

  // All students have boarded the bus, so release the lock
  pthread_mutex_unlock(&station->mutex);
}

int
station_wait_for_bus(struct station *station, int myticket, int myid)
{
  pthread_mutex_lock(&station->mutex);
  station->waiting_students++;

  // Wait for a bus to arrive and for it to be my turn to board
  while (myticket != station->boarding_turn + 1 || station->free_seats <= 0) {
    pthread_cond_wait(&station->bus_arrived, &station->mutex);
  }

  // It's my turn to board the bus, so board it
  station->boarding_turn++;
  station->waiting_students--;
  station->free_seats--;

  // Signal that the bus is full and that all students have boarded
  pthread_cond_signal(&station->student_boarded);

  // All students have boarded the bus, so release the lock
  pthread_mutex_unlock(&station->mutex);

  return station->boarding_turn;
}
