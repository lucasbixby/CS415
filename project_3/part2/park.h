/*
* Description: Project 3 [ part1 / park.h ] for Duck Park v1.0
*
* Author: Lucas Bixby
*
* Date: 05/28/2026 ( last modified )
*/

/*
    Part 1: part1 header file:
    Header file for part 1 linking the helper functions to the main park.c 
    program for part1. 
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
 
/* ─── Simulation Parameters ─────────────────────────────────────────── */

typedef struct {
    int N;  // number of passenger threads 
    int C;  // number of car threads       
    int P;  // capacity per car            
    int W;  // car waiting period (seconds)
    int R;  // car ride duration (seconds) 
    int T;  // park open duration (seconds)
    int J;  // ride queue max size         
} SimParams;
 
/* ─── Shared State ──────────────────────────────────────────────────── */
 
// Timing 
extern time_t park_start;               
 
// Park open flag 
extern volatile int park_open;
 
// Ticket queue length ( how many passengers are waiting for a ticket ) 
extern int ticket_queue_len;
 
// Ride queue length ( how many passengers are waiting to board ) 
extern int ride_queue_len;
 
// Car state 
extern int car_passengers;             // passengers currently loaded in car      
extern time_t last_board_time;         // wall-clock time last passenger boarded  
 
// Synchronization primitives 
extern pthread_mutex_t ticket_mutex;   // serializes ticket booth              
extern pthread_mutex_t state_mutex;    // protects all shared state      
extern pthread_mutex_t print_mutex;    // serializes stdout writes             
extern pthread_cond_t load_cond;       // car signals passengers to board      
extern pthread_cond_t unload_cond;     // car signals passengers to unboard    
extern pthread_cond_t car_ready_cond;  // passenger signals car a rider joined 
extern pthread_cond_t ride_q_cond;     // passenger waits when ride queue full 
extern sem_t loading_bay;              // only 1 car may be in load() at once  
 
// Car lifecycle flags 
extern int loading_open;               // 1 = car is accepting boarders           
extern int unloading_open;             // 1 = car has signaled unload             
extern int passengers_unboarded;       // number of passengers that have unboarded 
 
// Global simulation parameters
extern SimParams sim;
 
/* ─── Thread Argument Types ──────────────────────────────────────────── */

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
 
#endif 