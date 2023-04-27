#include <pthread.h>

struct station {
  pthread_mutex_t lock;
  pthread_cond_t bus_arrived;
  pthread_cond_t student_ready;
  int num_seats;
  int num_students;
  int next_student;
  int *students;
};

void
station_init(struct station *station, int num_seats) {
  pthread_mutex_init(&station->lock, NULL);
  pthread_cond_init(&station->bus_arrived, NULL);
  pthread_cond_init(&station->student_ready, NULL);
  station->num_seats = num_seats;
  station->num_students = 0;
  station->next_student = 0;
  station->students = malloc(sizeof(int) * num_seats);
}

void
station_load_bus(struct station *station, int count) {
  pthread_mutex_lock(&station->lock);
  while (station->num_students == station->num_seats) {
    pthread_cond_wait(&station->bus_arrived, &station->lock);
  }
  for (int i = 0; i < count; i++) {
    station->students[station->next_student++] = i;
  }
  station->num_students -= count;
  pthread_cond_broadcast(&station->student_ready);
  pthread_mutex_unlock(&station->lock);
}

int
station_wait_for_bus(struct station *station, int myticket, int myid) {
  pthread_mutex_lock(&station->lock);
  while (myticket > station->next_student) {
    pthread_cond_wait(&station->student_ready, &station->lock);
  }
  int boarding_turn = station->students[myticket];
  station->students[myticket] = -1;
  pthread_mutex_unlock(&station->lock);
  return boarding_turn;
}
