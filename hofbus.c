// The station struct represents a bus station with relevant synchronization primitives,
// waiting students, and available seats on the bus.
struct station {
    pthread_mutex_t lock; // Mutex lock for protecting shared data
    pthread_cond_t bus_arrived; // Condition variable for signaling when a bus has arrived
    pthread_cond_t student_boarded; // Condition variable for signaling when a student has boarded the bus
    int freeSeats; // Number of free seats available on the bus
    int waitStudents; // Number of students waiting at the station
    int nextTicket; // The ticket number of the next student to board
    int counterTicket; // The counter for assigning the next ticket number
};

// Initialize the station with default values and initialize synchronization primitives
void station_init(struct station *station) {
    pthread_mutex_init(&station->lock, NULL); // Initialize the mutex lock
    pthread_cond_init(&station->bus_arrived, NULL); // Initialize the bus_arrived condition variable
    pthread_cond_init(&station->student_boarded, NULL); // Initialize the student_boarded condition variable
    station->freeSeats = 0; 
    station->waitStudents = 0; 
    station->nextTicket = 1; 
    station->counterTicket = 1; 
}

// Load the bus with students waiting at the station
void station_load_bus(struct station *station, int count) {
    pthread_mutex_lock(&station->lock); // Lock the mutex to protect shared data

    // If there are no free seats on the bus or no waiting students at the station, the bus leaves
    if (count == 0 || station->waitStudents == 0) {
        pthread_mutex_unlock(&station->lock); // Unlock the mutex
        return;
    }

    station->freeSeats = count; // Update the number of free seats on the bus

    pthread_cond_broadcast(&station->bus_arrived); // Signal all waiting students that the bus has arrived

    // Wait until all free seats are taken or all waiting students have boarded
    while (station->waitStudents > 0 && station->freeSeats > 0) {
        pthread_cond_wait(&station->student_boarded, &station->lock); // Wait for a student to board the bus
    }

    station->freeSeats = 0; // Reset the number of free seats
    pthread_mutex_unlock(&station->lock); // Unlock the mutex
}

// Student waits for the bus and boards when it's their turn
int station_wait_for_bus(struct station *station, int myticket, int myid) {
    pthread_mutex_lock(&station->lock); // Lock the mutex to protect shared data
    station->waitStudents++; // Increment the number of waiting students

    // Wait until it's the student's turn to board and there are free seats on the bus
    while (myticket != station->nextTicket || station->freeSeats == 0) {
        pthread_cond_wait(&station->bus_arrived, &station->lock); // Wait for the bus to arrive
    }

    station->waitStudents--; // Decrement the number of waiting students
    station->nextTicket++; // Increment the next ticket number
    station->freeSeats--; // Decrement the number of free seats on the bus

    pthread_cond_signal(&station->student_boarded); // Signal that the student has boarded the bus
    pthread_mutex_unlock(&station->lock); // Unlock the mutex


    return (myticket);
}