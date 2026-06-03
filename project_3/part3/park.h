/*
* Description: Project 3 [ part3 / park.h ] for Duck Park v3.0
*
* Author: Lucas Bixby
*
* Date: 06/02/2026 ( last modified )
*/

/*
    Part 3: part3 header file:
        Header file for part 3 linking the helper functions in helpers.c 
        to the main park.c program for part3. 
*/

#ifndef PARK_H
#define PARK_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
 
/* --- Simulation Parameters ------------------------------------------------ */

typedef struct {
    int N;          // number of passenger threads 
    int C;          // number of car threads       
    int P;          // capacity per car            
    int W;          // car waiting period (seconds)
    int R;          // car ride duration (seconds) 
    int T;          // park open duration (seconds)
    int J;          // ride queue max size         
} SimParams;
 
/* --- Shared State --------------------------------------------------------- */
 
// Timing 
extern time_t park_start;               
 
// Park open flag 
extern volatile int park_open;
 
// Ticket queue length ( how many passengers are waiting for a ticket ) 
extern int ticket_queue_len;
 
// Ride queue length ( how many passengers are waiting to board ) 
extern int ride_queue_len;

// part3: passenger activity counters ( protected by state_mutex )
extern int exploring;
extern int waiting_in_car;
extern int riding;
extern int loading_car_id;
extern int unloading_car_id;
 
// Car state 
extern int *car_passengers;            // array indexed by car id     
extern time_t last_board_time;         // wall-clock time last passenger boarded  

// part3: CarState enum to track active status 
typedef enum {
    CAR_WAITING,
    CAR_LOADING,
    CAR_RIDING,
    CAR_UNLOADING
} CarState;

// define arrays for the car status 
extern CarState *car_states;
extern int *car_load_counts;
 
// Synchronization primitives 
extern pthread_mutex_t ticket_mutex;   // serializes ticket booth              
extern pthread_mutex_t state_mutex;    // protects all shared state      
extern pthread_mutex_t print_mutex;    // serializes stdout writes      

// part3: definitions for the ticket and ride queue mutex
extern pthread_mutex_t ticket_queue_mutex;
extern pthread_mutex_t ride_queue_mutex;      
// part3: final statistics accumulators 
extern int total_passengers_served;    // incremented each time a passenger unboards
extern int total_rides;                // incremented each time a car departs
extern double total_ticket_queue_secs; // sum of each passenger's time waiting for ticket
extern double total_ride_queue_secs;   // sum of each passenger's time waiting to board
extern time_t *ticket_enter_time;      // when passenger entered ticket queue
extern time_t *ride_enter_time;        // when passenger entered ride queue


extern pthread_cond_t load_cond;       // car signals passengers to board      
extern pthread_cond_t unload_cond;     // car signals passengers to unboard    
extern pthread_cond_t car_ready_cond;  // passenger signals car a rider joined 
extern pthread_cond_t ride_q_cond;     // passenger waits when ride queue full 
extern sem_t loading_bay;              // only 1 car may be in load() at once  
 
// Car lifecycle flags 
extern int loading_open;               // 1 = car is accepting boarders           
extern int *unloading_open;            // 1 = car has signaled unload             
extern int *passengers_unboarded;      // number of passengers that have unboarded 
extern int *passenger_car;             // which car id each passenger is riding
 
// Global simulation parameters
extern SimParams sim;

// part3: logging arrays
extern int *ticket_queue;
extern int *ride_queue;
 
/* --- Thread Argument Types ------------------------------------------------- */

// Passenger type
typedef struct {
    int id;
} PassengerArg;
 
// Car type 
typedef struct {
    int id;
} CarArg;
 
/* ─── Helper Function Declarations ──────────────────────────────────── */

// Utility Functions
int  get_elapsed(void);
void log_event(const char *fmt, ...);
void log_system_state();
void enqueue(pthread_mutex_t queue_type, int *queue, int size, int id);
int dequeue(pthread_mutex_t queue_type, int *queue, int size);
 
// Passenger Functions 
void explore_park(int id);
void get_ride_ticket(int id);
void enter_ride_queue(int id);
void board_car(int id);
void unboard_car(int id);
 
// Car Functions 
void car_load(int id);
void car_run(int id);
void car_unload(int id);
 
// Thread Behaviors 
void *passenger_thread(void *arg);
void *car_thread(void *arg); 
void *monitor_thread();
 
#endif 