/*
* Description: Project 3 [ part1 / park.c ] for Duck Park v1.0
*
* Author: Lucas Bixby
*
* Date: 05/28/2026 ( last modified )
*/

/*
    Part 1: Single-Threaded Solution:
    Develop and test each component with a single passenger thread and a 
    single car thread to verify basic functionality. Terminal output should 
    reflect the state of the system includes actions made or status changes 
    for each thread.
*/

#include "park.h"
#include <stdarg.h>
 
/* ─── Global Definitions ─────────────────────────────────────────────── */
SimParams sim;
 
time_t park_start;
volatile int park_open = 1;
 
int ticket_queue_len = 0;
int ride_queue_len = 0;
int car_passengers = 0;
time_t last_board_time = 0;
 
pthread_mutex_t ticket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t load_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t unload_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t car_ready_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ride_q_cond = PTHREAD_COND_INITIALIZER;
sem_t loading_bay;   
 
int loading_open = 0;
int unloading_open = 0;
int passengers_unboarded = 0;
 
/* ─── Defaults ───────────────────────────────────────────────────────── */
#define DEFAULT_N 1 // single passenger thread for part 1 
#define DEFAULT_C 1 // single car thread fot part 1 
#define DEFAULT_P 1
#define DEFAULT_W 1
#define DEFAULT_R 3
#define DEFAULT_T 30
#define DEFAULT_J 5
 
/* ─── Print simulation config ────────────────────────────────────────── */
static void print_config(void) 
// prints the simulation configuration before executing
{
    printf("- Number of passenger threads: %d\n", sim.N);
    printf("- Number of cars: %d\n",              sim.C);
    printf("- Capacity per car: %d\n",            sim.P);
    printf("- Car waiting period: %d\n",          sim.W);
    printf("- Car ride duration: %d\n",           sim.R);
    printf("- Park duration: %d seconds\n",       sim.T);
    printf("- Max ride queue size: %d\n\n",       sim.J);
}
 
/* ─── Main ───────────────────────────────────────────────────────────── */
int main() //int argc, char *argv[]
// main execution of the simulation. Opens the park, launches threads, closes the park
{
    // Set defaults values for non-specified parameters upon execution 
    sim.N = DEFAULT_N;
    sim.C = DEFAULT_C;
    sim.P = DEFAULT_P;
    sim.W = DEFAULT_W;
    sim.R = DEFAULT_R;
    sim.T = DEFAULT_T;
    sim.J = DEFAULT_J;
 
    // Initialize loading bay semaphore — for part 1, only 1 car can load at a time 
    sem_init(&loading_bay, 0, 1);
 
    // Record simulation start time 
    park_start = time(NULL);
 
    // Print the config header
    print_config();
 
    // Define threads 
    pthread_t passenger, car;

    PassengerArg p_arg = { .id = 0 };
    CarArg       c_arg = { .id = 0 };
 
    // Launch single car thread first 
    if (pthread_create(&car, NULL, car_thread, &p_arg) != 0) {
        fprintf(stderr, "Error: failed to create car thread %d.\n", 0);
        return 1;
    }
 
    // Launch single passenger threads 
    if (pthread_create(&passenger, NULL, passenger_thread, &c_arg) != 0) {
        fprintf(stderr, "Error: failed to create passenger thread %d.\n", 0);
        return 1;
    }
 
    // Let the park run for T seconds, then close 
    sleep(sim.T);
    park_open = 0;
    printf("\n========== PARK CLOSED ==========\n\n");
 
    // Wake any threads blocked on condition variables so they can exit 
    pthread_cond_broadcast(&load_cond);
    pthread_cond_broadcast(&unload_cond);
    pthread_cond_broadcast(&car_ready_cond);
    pthread_cond_broadcast(&ride_q_cond);
 
    /* ────────── Join all threads ────────── */
    pthread_join(passenger, NULL);
    pthread_join(car, NULL);
 
    // Cleanup 
    pthread_mutex_destroy(&ticket_mutex);
    pthread_mutex_destroy(&state_mutex);
    pthread_mutex_destroy(&print_mutex);
    pthread_cond_destroy(&load_cond);
    pthread_cond_destroy(&unload_cond);
    pthread_cond_destroy(&car_ready_cond);
    pthread_cond_destroy(&ride_q_cond);
    sem_destroy(&loading_bay);
 
    return 0;
}