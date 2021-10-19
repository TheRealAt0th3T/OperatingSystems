## Project Number/Title 

* Author: Amara Tariq
* Class: CS452/CS552 [Operating Systems] Section #001

## Overview

This program implements mergesort using the pthread library. Serial_mergesort is the single thread version, 
while parallel_mergesort is the multithread version. The program generates a random array of n length, 
duplicates it, and sends one into serial_mergesort, and another in parallel_mergesort. The output is then 
the time it took for each version to sort the array. 

## Manifest

* Makefile - This builds the entire project, creating all executables, and can also clean up, in order to run the program. 
* mytests.c - This is the driver file, which also creates the array and the functions that time the sort. 
* lab.h - This header file includes function prototypes, which when added, allow files to be able to call functions listed here.
* lab.c - This file contains all the sorting functions where the array is actually processed. 
* runval.sh - This is a script file that runs valgrind to check for memory leaks and other errors in the project.

## Building the project

To compile the program, user must call:<br>
```make```

To run the program: <br>
``` ./mytests <input size> <number of threads> [<seed>] ```

To remove any object files and executables: <br>
```make clean```

To run Valgrind in order to run additional tests: <br> 
```./runval.sh``` <br>
or <br>
```valgrind --leak-check=full ./mytests <input size> <number of threads> [<seed>]```

## Features and usage

The user inputs two main arguments, the input size, which dictates the size of the array to be sorted, 
and the number of threads they would like to be used. This program features being able to take in that 
input size and then generate two random duplicate arrays. These arrays are then ran through both serial 
mergesort and parallel mergesort, with serial sorting with a single thread, while parallel takes in number 
of threads from the command line in order to then process and sort the array. The processing time to get 
sorted arrays is tracked and then output to the user to compare the different efficiencies.

## Testing

Before testing, I had to make sure the makefile had all the correct flags and would correctly compile the program. We were to add a single line to flag pthreads library. I was able to refer to the discussion post on canvas for the line that needed to be added. Though, I still had to read into how to properly add it to the makefile. Once added properly, the actual testing of the program could start. 

The first main tests were to make sure the program compiled correctly. As I was not very familiar with C, there were a lot of syntax errors and minor formatting errors (ie declaring an int outside of the for loop before using it). After successful compilation, I ran the program to see if it would either succeed at sorting, and thus generate output, or fail it. The main issue I kept having was segmentation faults. This was due to errors in how I was mallocing my variables, and I ended up getting help in the tutoring center to fully correct the statements. The other main error occured when testing with valgrind. I had a program that correctly sorted and printed a sorted array, but valgrind returned errors. After discussing with the TA and the professor, I was able to determine that all the errors occured from indexing issues. The function that generates the random array starts indexing at i=1, instead of the traditional i=0. This leads to the initial index 0 being NULL. I had made the assumption that indexing starts at 0, thus all my loops were running into errors when trying to read and sort NULL. 
It was a simple fix of just adding 1 to all the places where I moved through the array. 

## Known Bugs

No known bugs. Everything was fixed and valgrind returns with 0 errors. 

## Reflection and Self Assessment

One of the first issues I encountered was simply just understanding the logic that we needed to apply for this project. I ended up meeting with other classmates to try and outline what the logic would be. I was able to be more comfortable and build a plan for how I wanted to approach the project. The main things I looked up had to do with the C language itself, the pthread library, and pointers. I knew what I wanted to do, I just needed to keep looking up what I could use to get to there. Understanding how pthread_create created and handled threads determined what variables and functions I needed. Through this process, I ran into a lot of errors, which are mentioned above in the 'Testing' section. 

I found it challenging to work with the new pthreads library and the language of C. It was also challenging to take our discussions and readings in class, which taught me how to theoretically think through threads, and suddenly apply it to code, without any examples or other resources. I do wish I could've had more code examples shown, as I felt like we sped through information in order to talk about
locks. 

Another challenge I had was keeping track of pointers. I used to use them a lot when I coded in C++, and had decided that maybe that was the best way to follow all the data of the threads, especially since I was using structs. I think the mallocing statements here were something that I struggled with. This was something I also ended up getting help from people in the tutoring center, which I discuss above in the 
'Testing' section. 

## Sources used

https://stackoverflow.com/questions/26753957/how-to-dynamically-allocateinitialize-a-pthread-array
https://stackoverflow.com/questions/20480529/thread-struct-as-function-parameter-c/20481075
https://www.codegrepper.com/code-examples/c/How+to+pass+a+struct+value+to+a+pthread+in+c%3F
https://stackoverflow.com/questions/39300744/how-to-pass-a-struct-by-value-to-a-pthread
