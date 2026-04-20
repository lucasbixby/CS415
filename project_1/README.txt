
Author: Lucas Bixby [ lbixby : 952001359 ]
Date: 04/20/2026 

This program is a basic shell simulation for file management and 
navigation which supports the following set of UNIX-like commands

    - ls        ( list current directory )      [ Usage: ls ]
    - pwd       ( print working directory )     [ Usage: pwd ]
    - mkdir     ( make directory )              [ Usage: mkdir < name > ]
    - cd        ( change directory )            [ Usage: cd < dst > ]
    - cp        ( copy file )                   [ Usage: cp < src > < dst > ]
    - mv        ( move file )                   [ Usage: mv < src > < dst > ]
    - rm        ( remove file )                 [ Usage: rm < filename > ]
    - cat       ( display file content )        [ Usage: cat < filename > ]

First compile the program through the Makefile by running "make" in the project 
directory. This will generate the following object files and executables.

    string_parser.o
    command.o
    main.o
    pseudo-shell

There are two fundememtal ways to use this program. 

    [ file_mode ]:
    Allows for reading bash commands from a input file, then
    writes the results of each bash command to a specified output 
    file. 

    Execute in this mode by running;

        ./pseudo-shell -f input_file.txt > output_file.txt

    [ interactive_mode ]:
    Allows for active command-line input, and displays result 
    within the terminal.

    Execute in this mode by running;

        ./pseudo-shell

After you finnish you can optionally clean-up the generated files 
by running "make clean" in your project directory. 

