## Project 2/Threaded safe Bounded Library

* Author: Amara Tariq
* Class: CS452/CS552 [Operating Systems] Section #001

## Overview

For this program, we created a threadsafe version of an existing list library. This library would then be used for a producer-consumer problem. The created ThreadedsafeBoundedList.c file in wrapper-library completes the implementation of the thread-safe list library.

## Manifest

* Makefile - This builds the entire project, creating all executables, and can also clean, in order to run the program.
* pc.c - This is the driver file that runs the producer-consumer problem with which we are testing our library with.
* test-pc.sh - This is the test script created with custom parameters.
* test-mq-pc.sh - A script for multiqueued version, which was for graduate students (not applicable to me)
* ThreadsafeBoundedList.h - This header file includes function prototypes, which when added, allow files to be able to call functions listed here.
* ThreadsafeBoundedList.c - This file contains all the functions for the list library which we created.
* List.h - This was a provided header file which was also used for reference for some functions in the threadsafeboundedlist.c
* Node.h - Another provided header file which was also used for reference.

## Building the project

First off, make sure when git cloning this project from the repository that all the files are created in your local version. There were many issues with that initially, so double check that before continuing. If file architechture is wrong, after trying to build, it will be difficult to clean and reverse the project. 

To compile the program, user must call:<br>
```make```

To run the program itself: <br>
``` ./pc <poolsize> <#items/producer> <#producers> <#consumers> <sleep interval(microsecs)> ```

To run program script: <br>
``` ./test-pc.sh ```

To remove any object files and executables: <br>
```make clean```

## Features and usage

This program features a producer consumer problem. We have a doubly-linked list with a capacity limit. This is shared between multiple producers and consumers (the number of each is dictated by the parameters sent in by the user). Based off this, the proper amount of threads is created.

Producers each produce a set number of items (again dictated by the user input). The consumer will then consume these items, with the results printing the total produced item vs consumed, which should be the same. 

The featured library used to manage these threads was ThreadsafeBoundedList.c. This is where locks and conditional variables were placed to make sure multiple threads could not reach critical sections of code, thus allowing them to run simultaneously without error. 

## Testing

Most of the errors were found from running the program. Initially I tried to just run the script to test, but found that because it ran pc so many times, I decided to just run pc with some custom inputs to test a smaller input size, ie pc 1 1 1 1 1. 

One of the first errors I had was with understanding the constructor and how to allocate memory for the tsb_list. I ended up going to the TA with that question, but even the constructor she gave me was wrong. After talking to the professor, I was able to correct the constructor to what it should've been. Where I was wrong with it was that I was trying to malloc memory for tsb_list, list, and node*capacity. The last two items had already been malloced for by other libraries, so that meant that I just needed to malloc only the struct of tsb_list. 

Another error I had was where I was trying to signal the conditional variables. I had initially put them inside the locks, but later found that they should be outside due to the fact that they aren't part of the critical sections. 

After fixing the errors mentioned above, I was not only able to compile correctly, but then the output of program was also correct.  

## Known Bugs

No known bugs.

## Reflection and Self Assessment

The first major issue was that for some reason, everytime I cloned the repository, I was missing random files and folders. I also only noticed this near the end when I was trying to build the project but it kept running errors on the makefile. Luckily, after just manually being able to go in and copy the missing files, I was able to get the program to run. 

For coding, the initial confusions I had was understanding which conditional variable was for which types of functions. I think this was partly due to the fact that they were named so similarly, but after conversing with some other students, I was able to figure out and keep in mind that listNotEmpty was for producers, and listNotFull was for consumers. After that, I was able to signal all the correct conditional variables. 

Another one was with the constructor, which I was able to figure out, as mentioned above in "Testing." 

I felt like this project was a lot simpler than project 1, especially since we didn't have to actually code the functions, due to having List.h with the functions we needed to call. As long as you knew how to build the locks, the conditions to put a thread to sleep, and how to signal conditional variables, it was a pretty easy project. I also appreciated that we were able to go over some of the basic functions in class because seeing the logic helped me think through the rest of them. 

## Sources used

Mainly used notes provided in class and the TA. 