/*
* Description: Project 3 [ part1 / helpers.c ] for Duck Park v1.0
*
* Author: Lucas Bixby
*
* Date: 06/02/2026 ( last modified )
*/

/*
    Part 1: helper functions
        contains the funcitonality and behaviors for passenger and car threads, 
        along with utility helper functions.
*/

#include "park.h"
#include <stdarg.h>
 
/* --- Utility Functions ------------------------------------------------ */
 
int get_elapsed(void) 
//  Returns elapsed time in seconds after the park has opened. 
{
    return (int)(time(NULL) - park_start);
}
 
void log_event(const char *fmt, ...) 
//  Thread-safe timestamped log for arbitrary event (passenger/ car)
{
    pthread_mutex_lock(&print_mutex);
    va_list args;
    printf("[Time: %d] ", get_elapsed());
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}
 
/* --- Passenger Functions ---------------------------------------------- */
 
void explore_park(int id) 
//  allow for pasenger to explore the park for a random time interval ( 1-10 seconds ) 
{
    log_event("Passenger %d is exploring the park", id);
    int explore_time = (rand() % 10) + 1; 
    sleep(explore_time);
    if (!park_open) pthread_exit(NULL);
    log_event("Passenger %d finished exploring, entering the ticket booth", id);
}
 
void get_ride_ticket(int id)  
//  allows for passengers to collect tickets at the ticket booth when its their turn.
//  Only one passenger can use the ticket booth at a time, simulates ticket processing 
//  time and accounts for park closing while passengers waiting in line. 
{
    pthread_mutex_lock(&ticket_mutex);
 
    if (!park_open) {
        pthread_mutex_unlock(&ticket_mutex);
        pthread_exit(NULL);
    }
 
    pthread_mutex_lock(&state_mutex);

    ticket_queue_len++;
    log_event("Passenger %d entering the ticket queue", id);
 
    // Block if ride queue is full — ticket cannot be issued 
    while (ride_queue_len >= sim.J && park_open) {
        pthread_cond_wait(&ride_q_cond, &state_mutex);
    }
 
    if (!park_open) {
        ticket_queue_len--;
        pthread_mutex_unlock(&state_mutex);
        pthread_mutex_unlock(&ticket_mutex);
        pthread_exit(NULL);
    }
 
    // Simulate ticket processing time 
    pthread_mutex_unlock(&state_mutex);
    sleep(2);
    pthread_mutex_lock(&state_mutex);

    if (!park_open) {
        ticket_queue_len--;
        pthread_mutex_unlock(&state_mutex);
        pthread_mutex_unlock(&ticket_mutex);
        pthread_exit(NULL);
    }
 
    ticket_queue_len--;
    log_event("Passenger %d acquired a ticket", id);
    pthread_mutex_unlock(&state_mutex);
 
    pthread_mutex_unlock(&ticket_mutex);
}
 
void enter_ride_queue(int id) 
//  allows passenger to enter the ride queue 
{
    pthread_mutex_lock(&state_mutex);
    ride_queue_len++;
    log_event("Passenger %d has entered the ride queue", id);
    pthread_mutex_unlock(&state_mutex);
}
 
void board_car(int id) 
//  handle passenger boarding the ride by waiting for loading_open signal, 
//  and send signals for boarding passengers. 
{
    pthread_mutex_lock(&state_mutex);
 
    while (!loading_open && park_open) {
        pthread_cond_wait(&load_cond, &state_mutex);
    }
 
    if (!park_open) {
        pthread_mutex_unlock(&state_mutex);
        pthread_exit(NULL);
    }
 
    car_passengers++;
    ride_queue_len--;
    last_board_time = time(NULL);
    log_event("Passenger %d is boarding", id);
 
    pthread_cond_signal(&car_ready_cond);
 
    pthread_mutex_unlock(&state_mutex);
}
 
void unboard_car(int id) 
//  handle passenger unboarding the ride. wait untill car is available to load. 
{
    pthread_mutex_lock(&state_mutex);
 
    while (!unloading_open && park_open) {
        pthread_cond_wait(&unload_cond, &state_mutex);
    }
 
    passengers_unboarded++;
    log_event("Passenger %d unboarded", id);
    pthread_cond_signal(&unload_cond);
    pthread_mutex_unlock(&state_mutex);
}
 
/* --- Car Functions ---------------------------------------------------- */
 
void car_load(int id) 
//  load passengers into car, one car at a time. Wait untill car is full 
//  or semi-full and W seconds have passed. Release the loading bay after 
//  all passengers are aboard and the doors are closed. The next car can 
//  start loading while this one rides.
{ 
    sem_wait(&loading_bay);
 
    pthread_mutex_lock(&state_mutex);
 
    // Reset all per-trip states before opening doors 
    loading_open = 1;
    unloading_open = 0;   
    passengers_unboarded = 0;
    car_passengers = 0;
 
    log_event("Car %d invoked load()", id);
 
    pthread_cond_broadcast(&load_cond);
 
    // Wait until full OR after sim.W seconds have passed 
    while (park_open) {
        if (car_passengers >= sim.P) {
            // Car is full —> depart immediately 
            break;
        }
 
        if (car_passengers > 0) {
            // At least one passenger 
            struct timespec deadline;
            clock_gettime(CLOCK_REALTIME, &deadline);
            deadline.tv_sec += sim.W;
 
            int rc = pthread_cond_timedwait(&car_ready_cond, &state_mutex, &deadline);
 
            if (car_passengers >= sim.P) break;          
            if (rc != 0 && car_passengers > 0) break;  
        } else {
            // No passengers —> wait indefinitely 
            pthread_cond_wait(&car_ready_cond, &state_mutex);
        }
    }
 
    if (!park_open) {
        loading_open = 0;
        pthread_mutex_unlock(&state_mutex);
        sem_post(&loading_bay);
        pthread_exit(NULL);
    }
 
    loading_open = 0;
    log_event("Car %d is full with %d passengers", id, car_passengers);
 
    pthread_mutex_unlock(&state_mutex);
       
    sem_post(&loading_bay);
}
 
void car_run(int id) 
//  run car event for sim.R ammount of time 
{
    log_event("Car %d has departed to ride", id);
    sleep(sim.R); 
    if (!park_open) {
        return;
    }
    log_event("Car %d has returned from the ride", id);
}
 
void car_unload(int id) 
//  unloaad passengers from car. Wait until every passanger has unboarded 
//  before signaling that loading is open 
{
    pthread_mutex_lock(&state_mutex);

    if (!park_open) {                    
        pthread_mutex_unlock(&state_mutex);
        return;
    }
 
    unloading_open = 1;
    int total = car_passengers;
 
    log_event("Car %d has invoked unload()", id);
    pthread_cond_broadcast(&unload_cond);
 
    // Wait until every passenger has unboarded 
    while (passengers_unboarded < total && park_open) {
        pthread_cond_wait(&unload_cond, &state_mutex);
    }
 
    unloading_open = 0;
 
    // Signal ride queue that spots may have opened 
    pthread_cond_broadcast(&ride_q_cond);
 
    pthread_mutex_unlock(&state_mutex);
}
 
/* --- Thread Behaviors ------------------------------------------------- */
 
void *passenger_thread(void *arg) 
//  main behavior of pasanger thread. Continues while the park is still open 
{
    PassengerArg *parg = (PassengerArg *)arg;
    int id = parg->id;
 
    log_event("Passenger %d entered the park", id);
 
    while (park_open) 
    {
        if (!park_open) break;
        explore_park(id);
 
        if (!park_open) break;
        get_ride_ticket(id);   
 
        if (!park_open) break;
        enter_ride_queue(id);
 
        if (!park_open) break;
        board_car(id);
 
        if (!park_open) break;
        unboard_car(id);
    }
 
    pthread_exit(NULL);
}
 
void *car_thread(void *arg) 
//  main behavior for car thread. Continues while the park is open
{
    CarArg *carg = (CarArg *)arg;
    int id = carg->id;
 
    while (park_open) 
    {
        car_load(id);
        if (!park_open) break;
 
        car_run(id);
        if (!park_open) break;

        car_unload(id);
    }
 
    pthread_exit(NULL);
}