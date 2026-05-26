/*
 * Lab 6 - Part 2: Pipes and Thread Communication
 *
 * Goal:
 *   Complete this file so that multiple worker threads update a shared counter
 *   and then write completion messages to a pipe. The main thread should read
 *   from the pipe and print the messages.
 *
 * What students should learn:
 *   1. How to create a pipe using pipe()
 *   2. How to write data to a pipe using write()
 *   3. How to read data from a pipe using read()
 *   4. How pipes can be used for communication
 *   5. How mutexes protect shared resources
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define NUM_WORKERS 10
#define INCREMENTS_PER_THREAD 100
#define MESSAGE_SIZE 128

/* Global shared data */
pthread_t *thread_ids;
pthread_mutex_t counter_lock;
pthread_mutex_t pipe_lock;
int counter = 0;
int pipe_fd[2];   /* pipe_fd[0] = read end, pipe_fd[1] = write end */

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
    if (thread_ids == NULL) {
        perror("malloc");
        exit(1);
    }


    /*
     * STEP 2: Initialize the mutexes.
     *
     * TODO:
     *   - Initialize counter_lock using pthread_mutex_init().
     *   - Initialize pipe_lock using pthread_mutex_init().
     *
     * Why two locks?
     *   - counter_lock protects the shared counter.
     *   - pipe_lock prevents multiple threads from writing mixed messages at the same time.
     */
    pthread_mutex_init(&counter_lock, NULL);
    pthread_mutex_init(&pipe_lock, NULL);

    /*
     * STEP 3: Create the pipe.
     *
     * TODO:
     *   - Call pipe(pipe_fd).
     *   - If pipe() returns -1, print an error with perror() and exit.
     *
     * Reminder:
     *   - pipe_fd[0] is used for reading.
     *   - pipe_fd[1] is used for writing.
     */
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }
    

    /*
     * STEP 4: Create an integer ID for each thread.
     *
     * TODO:
     *   - Create an array of NUM_WORKERS integers.
     *   - Fill the array with values 0 through NUM_WORKERS - 1.
     */
    int *numbers = malloc(sizeof(int) * NUM_WORKERS);
    if (numbers == NULL) {
        perror("malloc");
        exit(1);
    }
    for (int i = 0; i < NUM_WORKERS; i++) {
        numbers[i] = i;
    }


    /*
     * STEP 5: Create the worker threads.
     *
     * TODO:
     *   - Use pthread_create() to create NUM_WORKERS threads.
     *   - Each thread should run simulate_work().
     *   - Pass the address of the matching thread ID to each thread.
     */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&thread_ids[i], NULL, simulate_work, (void*)&numbers[i]);
    }


    /*
     * STEP 6: Wait for all worker threads to finish.
     *
     * TODO:
     *   - Use pthread_join() for each thread.
     *
     * Why wait before reading?
     *   - In this starter design, the main thread reads all messages after workers finish.
     */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    /*
     * STEP 7: Close the write end of the pipe in main.
     *
     * TODO:
     *   - Close pipe_fd[1] after all worker threads have finished writing.
     *
     * Why:
     *   - read() needs the write end closed so it can eventually return 0, meaning EOF.
     */
    close(pipe_fd[1]);


    /*
     * STEP 8: Read messages from the pipe.
     *
     * TODO:
     *   - Create a character buffer.
     *   - Use read(pipe_fd[0], buffer, sizeof(buffer)) inside a loop.
     *   - Print the bytes that were read.
     *
     * Hint:
     *   - read() does not automatically add '\0' to make a C string.
     *   - You may use fwrite(buffer, 1, bytes_read, stdout) to print safely.
     */
    char buffer[MESSAGE_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }


    /*
     * STEP 9: Print the final counter value.
     *
     * TODO:
     *   - Print the value of counter.
     *   - If synchronization is correct, the expected value is:
     *       NUM_WORKERS * INCREMENTS_PER_THREAD
     */
    printf("Final counter value: %d\n", counter);

    /*
     * STEP 10: Clean up resources.
     *
     * TODO:
     *   - Destroy both mutexes.
     *   - Close the read end of the pipe.
     *   - Free the thread_ids array.
     */
    pthread_mutex_destroy(&counter_lock);
    pthread_mutex_destroy(&pipe_lock);
    close(pipe_fd[0]);
    free(thread_ids);
    free(numbers);


    return 0;
}

void* simulate_work(void* arg) {

    /*
     * STEP 11: Convert the generic void* argument back to an int*.
     *
     * TODO:
     *   - Cast arg to int*.
     *   - Use this value as the thread's ID.
     */
    int id = *(int*)arg;
    printf("Thread %d started.\n", id);


    /*
     * STEP 12: Simulate work while safely updating the counter.
     *
     * TODO:
     *   - Repeat INCREMENTS_PER_THREAD times.
     *   - Optionally call usleep() inside the loop to make thread interleaving easier to see.
     *   - Lock counter_lock.
     *   - Increment counter.
     *   - Unlock counter_lock.
     */
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        usleep(1000);

        pthread_mutex_lock(&counter_lock);
        counter++;
        pthread_mutex_unlock(&counter_lock);
    }
    printf("Thread %d updated the shared counter.\n", id);

    /*
     * STEP 13: Prepare a completion message.
     *
     * TODO:
     *   - Create a character array called message.
     *   - Use snprintf() to create a message like:
     *       Thread <id> finished work. Counter is now <counter>.
     */
    char message[MESSAGE_SIZE];
    snprintf(message, sizeof(message),
             "Message from pipe: Thread %d completed work.\n",
             id);

    pthread_mutex_lock(&counter_lock);
    int current_counter = counter;
    pthread_mutex_unlock(&counter_lock);

    snprintf(message, sizeof(message),
             "Thread %d finished work. Counter is now %d.\n",
             id, current_counter);


    /*
     * STEP 14: Write the completion message to the pipe.
     *
     * TODO:
     *   - Lock pipe_lock before writing.
     *   - Use write(pipe_fd[1], message, strlen(message)).
     *   - Unlock pipe_lock after writing.
     *
     * Why:
     *   If multiple threads write at the same time, their messages may become mixed.
     */
    pthread_mutex_lock(&pipe_lock);

    write(pipe_fd[1], message, strlen(message));

    pthread_mutex_unlock(&pipe_lock);

    printf("Thread %d wrote a message to the pipe.\n", id);


    /*
     * STEP 15: Exit the thread.
     *
     * TODO:
     *   - Use pthread_exit(NULL), or simply return NULL.
     */
    printf("Thread %d finished.\n", id);

    return NULL;
}
