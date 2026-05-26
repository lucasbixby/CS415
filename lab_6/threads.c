/*
 * Lab 6 - Part 1: Threads and Mutex Synchronization
 *
 * Goal:
 *   Complete this file so that it creates multiple worker threads.
 *   Each worker thread should update a shared counter safely using a mutex.
 *
 * What students should learn:
 *   1. How to create threads with pthread_create()
 *   2. How to pass an argument to a thread function
 *   3. How to protect shared data using pthread_mutex_lock()/unlock()
 *   4. How to wait for threads using pthread_join()
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_WORKERS 10
#define INCREMENTS_PER_THREAD 100

/* Global shared data */
pthread_t *thread_ids;
pthread_mutex_t counter_lock;
int counter = 0;

/* Function prototype for the worker thread */
void* simulate_work(void* arg);

int main(int argc, char* argv[]) {

    /*
     * STEP 1: Allocate space for NUM_WORKERS thread IDs.
     *
     * TODO:
     *   - Use malloc() to allocate an array of pthread_t values.
     *   - Store the returned pointer in thread_ids.
     *   - Check if malloc() failed. If it failed, print an error and exit.
     */

    thread_ids = malloc(sizeof(pthread_t) * NUM_WORKERS);
    if (thread_ids == NULL){
        perror("malloc");
        exit(1);
    }

    /*
     * STEP 2: Initialize the mutex.
     *
     * TODO:
     *   - Use pthread_mutex_init() to initialize counter_lock.
     *   - The second argument can be NULL for default mutex attributes.
     */

    pthread_mutex_init(&counter_lock, NULL); 

    /*
     * STEP 3: Create an integer ID for each thread.
     *
     * TODO:
     *   - Create an array of NUM_WORKERS integers.
     *   - Fill the array with values 0 through NUM_WORKERS - 1.
     *   - These IDs will be passed to the worker threads.
     */

    int* numbers = malloc(sizeof(int) * NUM_WORKERS);
    if (numbers == NULL){
        perror("malloc");
        exit(1);
    }
    for(int i = 0; i < NUM_WORKERS; i++){
        numbers[i] = i; 
    }

    /*
     * STEP 4: Create the worker threads.
     *
     * TODO:
     *   - Use a for loop from 0 to NUM_WORKERS - 1.
     *   - Inside the loop, call pthread_create().
     *   - Pass simulate_work as the function each thread should run.
     *   - Pass the address of the matching thread ID as the argument.
     *
     * Hint:
     *   pthread_create(&thread_ids[i], NULL, simulate_work, (void*)&numbers[i]);
     */

    for (int i = 0; i < NUM_WORKERS; i++){
        pthread_create(&thread_ids[i], NULL, simulate_work, (void*)&numbers[i]);
    }

    /*
     * STEP 5: Wait for all worker threads to finish.
     *
     * TODO:
     *   - Use a for loop from 0 to NUM_WORKERS - 1.
     *   - Inside the loop, call pthread_join() on each thread.
     */

    for (int i = 0; i < NUM_WORKERS; i++){
        pthread_join(thread_ids[i], NULL);
    }

    /*
     * STEP 6: Print the final counter value.
     *
     * TODO:
     *   - Print the value of counter.
     *   - If everything is correct, the expected value is:
     *       NUM_WORKERS * INCREMENTS_PER_THREAD
     */

    printf("Final counter value: %d \n", counter);

    /*
     * STEP 7: Clean up resources.
     *
     * TODO:
     *   - Destroy the mutex using pthread_mutex_destroy().
     *   - Free the thread_ids array.
     */

    pthread_mutex_destroy(&counter_lock);
    free(thread_ids);
    free(numbers);

    return 0;
}

void* simulate_work(void* arg) {

    /*
     * STEP 8: Convert the generic void* argument back to an int*.
     *
     * TODO:
     *   - Cast arg to int*.
     *   - Use this value as the thread's ID for printing messages.
     */
    int id = *(int*)arg;

    /*
     * STEP 9: Print that this thread has started.
     *
     * TODO:
     *   - Print a message like:
     *       Thread <id> started.
     */
    printf("Thread %d started.\n", id);

    /*
     * STEP 10: Simulate some work.
     *
     * TODO:
     *   - Call sleep(1) or usleep() to make thread scheduling easier to observe.
     */
    sleep(1);

    /*
     * STEP 11: Safely update the shared counter.
     *
     * TODO:
     *   - Lock counter_lock before modifying counter.
     *   - Increment counter INCREMENTS_PER_THREAD times.
     *   - Unlock counter_lock after modifying counter.
     *
     * Why:
     *   counter is shared by all threads. Without a mutex, a race condition can occur.
     */
    pthread_mutex_lock(&counter_lock);

    printf("Thread %d is updating the shared counter\n", id);
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        counter++;
    }

    pthread_mutex_unlock(&counter_lock);

    /*
     * STEP 12: Print that this thread has finished.
     *
     * TODO:
     *   - Print a message like:
     *       Thread <id> finished.
     */
    printf("Thread %d finished.\n", id);

    /*
     * STEP 13: Exit the thread.
     *
     * TODO:
     *   - Use pthread_exit(NULL), or simply return NULL.
     */

    return NULL;
}
