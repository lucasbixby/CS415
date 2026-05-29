#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;
int counter = 0;

int increments_per_thread;

void *worker(void *arg)
{
    (void)arg;

    /* TODO: Loop increments_per_thread times. */
    /* TODO: Lock the mutex before updating counter. */
    /* TODO: Increment counter by 1. */
    /* TODO: Unlock the mutex after updating counter. */
    for (int i = 0 ; i < increments_per_thread ; i++){
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int num_threads;
    pthread_t *threads;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_threads> <increments_per_thread>\n", argv[0]);
        return 1;
    }

    num_threads = atoi(argv[1]);
    increments_per_thread = atoi(argv[2]);

    if (num_threads <= 0 || increments_per_thread <= 0) {
        fprintf(stderr, "num_threads and increments_per_thread must be positive\n");
        return 1;
    }

    threads = malloc((size_t)num_threads * sizeof(pthread_t));
    if (threads == NULL) {
        perror("malloc");
        return 1;
    }

    /* TODO: Initialize the mutex (pthread_mutex_init). */
    pthread_mutex_init(&mutex, NULL);

    /* TODO: Create num_threads threads, each running worker. */
    for (int i=0; i < num_threads ; i++){
        pthread_create(&threads[i] , NULL, worker, NULL);
    }

    /* TODO: Join all threads. */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", counter);

    /* TODO: Destroy the mutex (pthread_mutex_destroy). */
    pthread_mutex_destroy(&mutex);

    free(threads);
    return 0;
}
