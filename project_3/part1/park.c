/*
* Description: Project 3 [ part 1 ] for Duck Park v1.0
*
* Author: Lucas Bixby
*
* Date: 05/26/2026 ( last modified )
*/

/*
    Part 1: Single-Threaded Solution:
    Develop and test each component with a single passenger thread and a single car thread to verify 
    basic functionality. Terminal output should reflect the state of the system includes actions made 
    or status changes for each thread.
*/


#include "park.h"
#include <stdarg.h>
 
/* ─── Global Definitions ─────────────────────────────────────────────── */
SimParams g;
 
time_t park_start;
volatile int park_open = 1;
 
int ticket_queue_len   = 0;
int ride_queue_len     = 0;
int car_passengers     = 0;
time_t last_board_time = 0;
 
pthread_mutex_t ticket_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state_mutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  load_cond      = PTHREAD_COND_INITIALIZER;
pthread_cond_t  unload_cond    = PTHREAD_COND_INITIALIZER;
pthread_cond_t  car_ready_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  ride_q_cond    = PTHREAD_COND_INITIALIZER;
sem_t           loading_bay;   /* initialized in main to 1                    */
 
int loading_open         = 0;
int unloading_open       = 0;
int passengers_unboarded = 0;
 
/* ─── Defaults ───────────────────────────────────────────────────────── */
#define DEFAULT_N 1
#define DEFAULT_C 1
#define DEFAULT_P 2
#define DEFAULT_W 3
#define DEFAULT_R 2
#define DEFAULT_T 30
#define DEFAULT_J 5
 
/* ─── Usage ──────────────────────────────────────────────────────────── */
static void print_usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "Options:\n"
        "  -n <int>   number of passenger threads     (default: %d)\n"
        "  -c <int>   number of car threads           (default: %d)\n"
        "  -p <int>   capacity per car                (default: %d)\n"
        "  -w <int>   car waiting period (seconds)    (default: %d)\n"
        "  -r <int>   car ride duration (seconds)     (default: %d)\n"
        "  -t <int>   park open duration (seconds)    (default: %d)\n"
        "  -j <int>   ride queue max size             (default: %d)\n"
        "  -h         display this help message\n",
        prog,
        DEFAULT_N, DEFAULT_C, DEFAULT_P,
        DEFAULT_W, DEFAULT_R, DEFAULT_T, DEFAULT_J
    );
}
 
/* ─── Print simulation config ────────────────────────────────────────── */
static void print_config(void) {
    printf("- Number of passenger threads: %d\n", g.N);
    printf("- Number of cars: %d\n",              g.C);
    printf("- Capacity per car: %d\n",            g.P);
    printf("- Car waiting period: %d\n",          g.W);
    printf("- Car ride duration: %d\n",           g.R);
    printf("- Park duration: %d seconds\n",       g.T);
    printf("- Max ride queue size: %d\n\n",       g.J);
}
 
/* ─── Main ───────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
 
    /* Set defaults */
    g.N = DEFAULT_N;
    g.C = DEFAULT_C;
    g.P = DEFAULT_P;
    g.W = DEFAULT_W;
    g.R = DEFAULT_R;
    g.T = DEFAULT_T;
    g.J = DEFAULT_J;
 
    /* Parse command-line flags with getopt */
    int opt;
    while ((opt = getopt(argc, argv, "n:c:p:w:r:t:j:h")) != -1) {
        switch (opt) {
            case 'n': g.N = atoi(optarg); break;
            case 'c': g.C = atoi(optarg); break;
            case 'p': g.P = atoi(optarg); break;
            case 'w': g.W = atoi(optarg); break;
            case 'r': g.R = atoi(optarg); break;
            case 't': g.T = atoi(optarg); break;
            case 'j': g.J = atoi(optarg); break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
 
    /* Basic validation */
    if (g.N <= 0 || g.C <= 0 || g.P <= 0 ||
        g.W <= 0 || g.R <= 0 || g.T <= 0 || g.J <= 0) {
        fprintf(stderr, "Error: all parameters must be positive integers.\n");
        print_usage(argv[0]);
        return 1;
    }
    if (g.P >= g.N && g.N > 1) {
        fprintf(stderr, "Warning: P should be < N per spec (P=%d, N=%d).\n", g.P, g.N);
    }
 
    /* Initialize loading bay semaphore — only 1 car loads at a time */
    sem_init(&loading_bay, 0, 1);
 
    /* Record simulation start time */
    park_start = time(NULL);
 
    /* Print config header */
    print_config();
 
    /* ── Allocate thread handles & arguments ── */
    pthread_t *passenger_threads = malloc(g.N * sizeof(pthread_t));
    pthread_t *car_threads       = malloc(g.C * sizeof(pthread_t));
    PassengerArg *p_args         = malloc(g.N * sizeof(PassengerArg));
    CarArg       *c_args         = malloc(g.C * sizeof(CarArg));
 
    if (!passenger_threads || !car_threads || !p_args || !c_args) {
        fprintf(stderr, "Error: failed to allocate thread memory.\n");
        return 1;
    }
 
    /* ── Launch car threads first so cars are ready to load ── */
    for (int i = 0; i < g.C; i++) {
        c_args[i].id = i;
        if (pthread_create(&car_threads[i], NULL, car_thread, &c_args[i]) != 0) {
            fprintf(stderr, "Error: failed to create car thread %d.\n", i);
            return 1;
        }
    }
 
    /* ── Launch passenger threads ── */
    for (int i = 0; i < g.N; i++) {
        p_args[i].id = i;
        if (pthread_create(&passenger_threads[i], NULL, passenger_thread, &p_args[i]) != 0) {
            fprintf(stderr, "Error: failed to create passenger thread %d.\n", i);
            return 1;
        }
    }
 
    /* ── Let the park run for T seconds, then close ── */
    sleep(g.T);
    park_open = 0;
    printf("\n========== PARK CLOSED ==========\n\n");
 
    /* Wake any threads blocked on condition variables so they can exit */
    pthread_cond_broadcast(&load_cond);
    pthread_cond_broadcast(&unload_cond);
    pthread_cond_broadcast(&car_ready_cond);
    pthread_cond_broadcast(&ride_q_cond);
    pthread_mutex_unlock(&ticket_mutex);   /* in case a passenger holds it     */
 
    /* ── Join all threads ── */
    for (int i = 0; i < g.N; i++) {
        pthread_join(passenger_threads[i], NULL);
    }
    for (int i = 0; i < g.C; i++) {
        pthread_join(car_threads[i], NULL);
    }
 
    /* ── Cleanup ── */
    pthread_mutex_destroy(&ticket_mutex);
    pthread_mutex_destroy(&state_mutex);
    pthread_mutex_destroy(&print_mutex);
    pthread_cond_destroy(&load_cond);
    pthread_cond_destroy(&unload_cond);
    pthread_cond_destroy(&car_ready_cond);
    pthread_cond_destroy(&ride_q_cond);
    sem_destroy(&loading_bay);
 
    free(passenger_threads);
    free(car_threads);
    free(p_args);
    free(c_args);
 
    return 0;
}