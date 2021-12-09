## Project 4/Buddy System

* Author: Amara Tariq
* Class: CS452/CS552 [Operating Systems] Section #001

## Overview

Concisely explain what the program does. If this exceeds a couple of
sentences, you're going too far. Generally you should be pulling this
right from the project specification. We don't want you to just cut and
paste, but paraphrase what is stated in the project specification.

For this project, we implemented our own memory management based on the buddy system. 


## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

* Makefile - This builds the entire project, creating all executables, and can also clean, in order to run the program.
* buddy.c - 
* buddy.h - 
* buddy-test.c - 
* buddy-unit-test.c - 
* malloc-test.c - 

## Building the project

This section should tell the user how to build your code.  If you are
delivering a library, where does it need to be installed or how do you use
it? Is this an executable, if so how can a user get up to speed as fast
as possible.

To compile the program, user must call:<br>
```make```

Then we want to make sure we export our library:<br>
``` export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libbuddy.so ```

To run program with our unit-tests:<br>
``` ./buddy-unit-test.c v ```

To run program with buddy-test.c:<br>
``` ./buddy-test.c <num of tests> [random seed] [silent|terse|verbose|interactive] ```

to run program with malloc-testing:<br>
``` ./malloc-test.c [][] ```


to run with mergesort
in preload -> run make -> .so file cpy that into project 1
make
run jidong thing


## Features and usage

Summarize the main features of your program. It is also appropriate to
instruct the user how to use your program.

This program features

## Testing

This section should detail how you tested your code. Simply stating "I ran
it a few times and it seems to work" is not sufficient. Your testing needs to
be detailed here.

One of the first functions I tested was buddy_init. Even thought it wasn't necessarily tested via running it, I ran through it with students in the tutoring lab to make sure the logic of it was sound. This is where we also found that we could use ceil and log2 to help make sure our size and kval were in line with the equation size = 2^kval. This was also a function where we had to go back and ended up changing ceil and log2 to bit manipulation based calculations. 



## Known Bugs

List known bugs that you weren't able to fix (or ran out of time to fix).

## Reflection and Self Assessment

Discuss the issues you encountered during development and testing. What
problems did you have? What did you have to research and learn on your
own? What kinds of errors did you get? How did you fix them?

What parts of the project did you find challenging? Is there anything that
finally "clicked" for you in the process of working on this project? How well
did the development and testing process go for you?

This project was tough. I felt like even though we got introduced to it before Thanksgiving break, we all still didn't understand enough or feel like we knew where to start. It wasn't until after break when we discussed the project more in depth in class where I felt like I had an idea of where to start and where to go with the project. 

The hardest function for me was buddy_init(), as you really had to have a proper understanding of how the blocks function inorder to know how to initialize them all. Especially with issues of the user inputted size not being one with a proper kval, there had to be checks to make sure and round up to the next power that would work as a kval. Initially I had done this via ceil and log2 functions, but the night before it was due we were told that either wouldn't work and we had to alter the makefile, or that it was just not recommended to take that path. It was frustrating that even till the last day there are things that are just now coming up and being solved. 

## Sources used

If you used any sources outside of the text book you should list them here. If you looked something up on
stackoverflow.com and fail to cite it in this section it will be considered plagiarism and be dealt with accordingly. So be safe CITE!

* any students in the tutoring lab that helped 
* the TA
* Alejandro Macias 
* Daniel Rao
*