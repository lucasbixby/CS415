Author: Lucas Bixby [ lbixby : 952001359 ]
Date: 06/02/2026

Duck Park is a multi-threaded amusement park simulation written in C using the
POSIX Threads (pthreads) library. The project is broken into 3 parts that each
build off of the previous codebase, progressively introducing more complex
synchronization, concurrent threads, and real-time monitoring.

================================================================================
File Structure:
================================================================================

Each part lives in its own directory ( part1/ part2/ part3/ ) with an output/
subdirectory for captured runs, and shares the same three-file layout:

    project_3/
    ├── part1/
    │   ├── output/output.txt
    │   ├── helpers.c   ( passenger & car thread behaviors, utility functions )
    │   ├── Makefile
    │   ├── park.c      ( globals, thread creation, park lifecycle )
    │   └── park.h      ( shared state, sync primitives, function prototypes )
    ├── part2/          ( same structure )
    ├── part3/          ( same structure )
    └── README.txt

The three-file split keeps park.c minimal and focused on the simulation
lifecycle, while helpers.c encapsulates all thread logic for readability.
park.h serves as the single source of truth for shared state across both files.

================================================================================
Part 1: Single-Threaded Foundation
================================================================================

Establishes the core simulation with one passenger thread and one car thread
to verify the basic lifecycle before introducing concurrency at scale.

    - Passenger lifecycle: explore -> ticket booth -> ride queue -> board -> unboard
    - Car lifecycle: load() -> run() -> unload(), repeating while park is open
    - Ticket booth serialized with ticket_mutex ( one passenger at a time )
    - Car loading gated by a semaphore ( loading_bay ) and condition variables
    - Park closes after T seconds, broadcasting all CVs to unblock waiting threads

================================================================================
Part 2: Multi-Threaded Scaling
================================================================================

Extends part 1 to N passenger threads and C car threads running concurrently,
stress-testing the synchronization design and hardening it against race
conditions that only emerge at scale.

    - Command-line flags: -n -c -p -w -r -t -j -h ( with defaults and validation )
    - Dynamic thread allocation via malloc for N passengers and C cars
    - park_open guard added inside car_load() immediately after sem_wait to
      prevent cars mid-load at close time from completing a post-close trip
    - pthread_cond_signal added to unboard_car() so the car wakes correctly
      when P > 1 passengers are unboarding ( latent deadlock fix from part 1 )

================================================================================
Part 3: IPC Monitoring
================================================================================

Introduces a monitor thread that reports live simulation state every 5 seconds
and prints a final statistics summary on park close.

    - monitor_thread() snapshots ticket queue, ride queue, and per-car status
    - CarState enum ( WAITING / LOADING / RIDING / UNLOADING ) with a
      car_states[] array indexed by car id for independent per-car tracking
    - ticket_queue[] / ride_queue[] arrays display queue contents by passenger id
    - Per-passenger timestamp arrays for computing average ticket and ride wait times
    - Final accumulators: total_passengers_served, total_rides, avg utilization

    Live output ( every 5s ):          Final output ( on close ):
    [Monitor] SYSTEM STATE =>          [Monitor] FINAL STATISTICS:
    Ticket Queue: [Passenger 5]        Total Simulation Time:        [Time: 60]
    Ride Queue: []                     Total Passengers Served:      22
    Car 0: RIDING  (2/2 Passengers)    Total Rides:                  14
    Car 1: WAITING (0/2 Passengers)    Average Ticket Queue Seconds: 2.6
    Passengers: 30 (2 exploring, ...)  Average Car Utilization:      78.6 Percent

================================================================================
Compilation and Execution:
================================================================================

Compile from within any part directory:

    make

Run with defaults or customize via flags ( part 2 and 3 only ):

    ./park
    ./park -n 30 -c 4 -p 2 -w 3 -r 2 -t 60 -j 10

Redirect output to the output file:

    ./park -n 30 -c 4 -t 60 > output/output.txt

Clean up compiled files:

    make clean