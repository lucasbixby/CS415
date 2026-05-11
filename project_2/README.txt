
Author: Lucas Bixby [ lbixby : 952001359 ]
Date: 05/10/2026 

This program is a simple MCP ( Master Control Program ) that handles workloads
from an input file. The project is broken into 4 parts that each build off of 
the previous codebase. 

Files in this project:

    - part1.c     [ Extracts the commands from an input file and launches the workload ]
    - part2.c     [ Allows for the MCP to control the flow of the processes ( SIGSTOP / SIGCONT ) ]
    - part3.c     [ Implements a Round-Robbin algorithm for process scheduling ]
    - part4.c     [ Displays a table of formatted process informaiton for every cycle ]

    - MCP.h       ( Header file for the project )           [ Contains: command_line struct, funciton declarations ]

    - helpers.c   ( Helper function implementations )       [ Contains: extracting commands, parsing, signals, memory 
                                                                        clean-up, enqueue/dequeue, process-schedular, 
                                                                        and displaying process information ]

    - Makefile    ( Compiles executables )                  [ Usage: "make", "make clean" ]

First compile the program through the Makefile by running "make" in the project 
directory. This will generate the following object files and executables.

    - part1
    - part2
    - part3
    - part4
    - iobound
    - cpubound

Execute the program by using file mode: 

    [ file_mode ]:
    Allows for reading commands from a input file, optionally
    writes the results of the workload to a specified output 
    file. 

    Execute in this mode by running:

        ./part_ -f input.txt 

    optionally write to an output file by running:

        ./part_ -f input.txt > output.txt

After you finnish you can optionally clean-up the generated files 
by executing "make clean" in your project directory. 
