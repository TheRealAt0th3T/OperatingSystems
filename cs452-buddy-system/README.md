## Project 4/Buddy System

* Author: Amara Tariq
* Class: CS452/CS552 [Operating Systems] Section #001

## Overview
For this project, we implemented our own memory management based on the buddy system. Buddy system is a style of memory managment where memory is incremented by powers of 2. Blocks of memory are split and merged together, making memory easier to manage and allocate. 

## Manifest

### non-preload: 
* Makefile - This builds the entire project, creating all executables, and can also clean, in order to run the program.
* buddy.c - This is the main driver file which contains the implementation of all our buddy functions (init, malloc, calloc, realloc, etc).
* buddy.h - This header file includes function prototypes, which when added, allow files to be able to call functions listed here. 
* buddy-test.c - This tester file tests to make sure the buddy_malloc() function works. The testing here is more detailed and will further test your program. 
* buddy-unit-test.c - This unit testing file contains tests that check the system malloc with 1 byte, max allocation, and other included unit tests. 

### preload:
* Makefile - This builds the entire project, creating all executables, and can also clean, in order to run the program.
* buddy.c - This is the main driver file with contains the implementation of our functions (malloc, calloc, realloc, etc). Differs from non-preload as these are made in order to export the library and use with other programs. 
* buddy.h - This is the header file which includes function prototypes, which when added, allows files to be able to call the functions listed here. 
* malloc-test.c - This tester file tests to make sure the malloc() function works. 

## Building the project

To compile the program, user must call:<br>
```make```

Then we want to make sure we export our library:<br>
``` export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libbuddy.so ```

To run program with our unit-tests:<br>
``` ./buddy-unit-test v ```

To run program with buddy-test.c:<br>
``` ./buddy-test <num of tests> [random seed] [silent|terse|verbose|interactive] ```

To run program with malloc-testing:<br>
``` ./malloc-test <num of tests> [random seed] [silent|terse|verbose|interactive]```

### Running with Mergesort

In order to test our library with mergesort, a copy of mergesort needs to be made and then cd into the main folder.

This is the location we want to cooy and paste the 'libbuddy.so' which is a file created by making within buddy-preload.

Once this file is in your mergesort folder follow these commands:
``` make ```
``` LD_PRELOAD=./libbuddy.so ./mytests 1000000 1 ```


## Features and usage

This program features a buddy system that has the following main functions: init, malloc, calloc, realloc, and free. These functions all work together to manage memory. 

User inputs a size, and then we can round the size up to be a power of 2 (ie user inputs 511 and program roounds it to 512 which is 2^9). By rounding up, the program is able to have space that is even and always divisible by two. This allows memory to easily be split into blocks, and merged back together. Splitting and merging occurs based on the need and can allow for more efficient usage of the memory. 

Usage is described above in the "building the project" section. 

## Testing

One of the first functions I tested was buddy_init. Even thought it wasn't necessarily tested via running it, I ran through it with students in the tutoring lab to make sure the logic of it was sound. This is where we also found that we could use ceil and log2 to help make sure our size and kval were in line with the equation size = 2^kval. This was also a function where we had to go back and ended up changing ceil and log2 to bit manipulation based calculations. This is also a function that really set up how the we approached the rest of the functions. 

The next functions of calloc and realloc were a lot easier for me. I knew exactly what checks needed to be done and how to do them, and endup just making sure with the TA if they were done properly. Malloc and free, on the otherhand, just caused trouble because there were a lot more moving parts to those functions. 

Once buddy-non-preload was completed, I began testing on it. It did take me a second to understand how to properly export the library and be able to run the tests, but luckily there were plenty of OS students in the lab that could help me. After following the instructions on how to compile, export library, and run, I found my program ran segmentation faults. I went back to the code to make sure arguments and conditionals were correct. I found that my error was with two return functions that I accidentally had forgotten to change from returning NULL. After that my non-preload ran fine. 

Preload was fairly straight forward, as all we had to do was to copy and paste our functions from non-preload and simply change the names from 'buddy_malloc()' to just 'malloc()'. There were no major issues with this step, since I already had tested and made sure non-preload worked. 

Testing in mergesort was a different experience all together. I didn't quite understand how we were supposed to implement mergesort into this project. I ended up having to go to the lab and asked another classmate on how to do it. Michael was able to help me with that. It did end up working, but I also wondered we knew the program was for sure using the new library we were exporting in. After attempting to remove the old library all together and getting errors, I did end up asking the professor this question. Turns out that the program would still need the old library because of how the tests and files check for that library existing. 

Another concern I had was that Michael was showing me how there is another way to make sure our library is working, and that is to call 'ls' command while running the export library command. His works, but on mine, it threw a segmentation error. After asking about this, I found that this is a very specific edge case. Though still throwing an error, I was told I shouldn't worry about it since Jidong will only be testing our project against mergesort. Since it works against that, I would be fine. 

## Known Bugs

None.

## Reflection and Self Assessment

This project was tough. I felt like even though we got introduced to it before Thanksgiving break, we all still didn't understand enough or feel like we knew where to start. It wasn't until after break when we discussed the project more in depth in class where I felt like I had an idea of where to start and where to go with the project. 

The hardest function for me was buddy_init(), as you really had to have a proper understanding of how the blocks function inorder to know how to initialize them all. Especially with issues of the user inputted size not being one with a proper kval, there had to be checks to make sure and round up to the next power that would work as a kval. Initially I had done this via ceil and log2 functions, but the night before it was due we were told that either wouldn't work and we had to alter the makefile, or that it was just not recommended to take that path. It was frustrating that even till the last day there are things that are just now coming up and being solved. 

I had issues understanding what the buddy_malloc() function needed and ended up having to spend a lot of time on understanding what the function would entail. Now looking back on it, it wasn't as difficult as I had maybe made it out to be, and I really do fully understand how it works. Buddy_free() was also on the harder end, but I feel like it is always a bit more complicated to fully free a block, especially with so many pointers and other things, like offsets with the headers within the blocks, that can make it seem more complex. 

## Sources used
* the TA
* Alejandro Macias 
* Daniel Rao
* https://cs.wellesley.edu/~cs240/f16/slides/allocator.pdf
* http://mpaxos.com/teaching/cso18spring/notes/14-Dynamic_Allocator.pdf