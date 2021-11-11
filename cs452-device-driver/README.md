
## Project 3/Device Driver

* Author: Amara Tariq 
* Class: CS452/CS552 [Operating Systems] Section #001

## Overview

Concisely explain what the program does. If this exceeds a couple of
sentences, you're going too far. Generally you should be pulling this
right from the project specification. We don't want you to just cut and
paste, but paraphrase what is stated in the project specification.

For this program, we wrote a device driver named booga. This driver supports open, read, write, and close operations, and returns a partial stream of one of four phrases, which is randomly chosen. 

This was a simple driver was kernel based. In order to run it, we had to use Vmware-vased CentOS. 

## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

* Makefile - This builds the entire project, creating executables, and also cleans, all in order to run the progam. 
* booga.c - This is our main file containing all the functions such as open, read, write, exit, etc. 
* booga.h - This is our header file which includes function prototypes, which when added, allow files to be able ot call functions listed here. 
* booga_load - This file loads the drivers. 
* booga_unload - This file unloads the drivers, removing any remaining content/nodes. 
* test-booga.c - This is our actual test file, which the test scripts call. 
* booga-test1.sh - This is our first test script which tests the regular functionalities of our driver. 
* booga-test2.sh - This is our second test script which tests if the driver is thread-safe. 

## Building the project

This section should tell the user how to build your code.  If you are
delivering a library, where does it need to be installed or how do you use
it? Is this an executable, if so how can a user get up to speed as fast
as possible.

First off, make sure you have a VM installed with CentOS 7 (64 bit) installed. This program will ONLY run on this. Also make sure you have root permissions and can run sudo commands. 

Clone the repository onto the VM and run the following command to compile: <br>
``` make ```

run this next command to prep the device drier and load:
``` sudo ./booga_load ```

The following command will show you how much space your driver is using: 
``` /sbin/lsmod ```

this command will read/write a specific buffer size and minor number:
``` ./test-booga <minor number> <buffer size> <read or write> ```

The following command will print to screen what information the driver has written to it:
``` cat /proc/driver/booga ```

To run the test script to check functionality:
``` sudo ./booga-test1.sh ```

To run the test script to check thread-safety:
``` sudo ./booga-test2.sh ```

## Features and usage

Summarize the main features of your program. It is also appropriate to
instruct the user how to use your program.

This program features a device driver. The driver is able to open, read, write, and close. It has 4 minor numbers, 0-3. 

The driver, upon reading one of the files (/dev/booga# ranging 0-3), will return a partial stream of a randomly chosen phrase from the 4 phrases provided. The partiality of it comes from the number of bytes the user requested to be read. 

When writing to devices 0-2, the driver works like /dev/null, pretending to write to a buffer but not really writing. This is because we are simple simulating a device driver. If the driver attempts to write to /dev/booga3, it will immediately be terminated. 

## Testing

This section should detail how you tested your code. Simply stating "I ran
it a few times and it seems to work" is not sufficient. Your testing needs to
be detailed here.

One of the main issues I had regarding running this program was that the only computer I own and code on is a Mac M1. This computers architecture doesn't allow VMware to run on it at all. Due to this, I had to blindly code and get the base of my code completed, and then have a classmate meet up with me and let me run my code and try and debug it on their system. 

Besides the typical spelling issues most coders have, I had two major concerns. One was the size of the moduel being off when running the command ``` cat /proc/driver/booga ```. This I later learned from the TA was quite normal, since the device would taken up differing space depending on how one codes it, the system its being run on, etc. 

The other major issue I had was with counting the amount of times phrases were printed. I believe this was all due to the 'long' that it was declared as. The error would occur if I ran the program more than once, the count would then randomly get negative, and not just a simple -1, it would go to large negative numbers. This reminded me of how in ECE we had learned about unsigned and signed ints, and it lead me to believing that was what was going on. In order to correct this, I split up the inital array of values I had (based on hving the minor number as the indexing) to individual variables.

I also, during that testing, had thought that the openCount[] array I had would also run into the same issues, so I also ended up splitting that up. Though, the next error I ran into caused me to revert it back to the array. The error I had was that when I ran the test scripts, it would print zeros for the count. I thought having split up the opencount might have messed something up, so I reverted the changes. Later I realized I simply forgot to change the return statement from returning 0 to returning count. After that my program was fully functioning. 

## Known Bugs

No known bugs. 

## Reflection and Self Assessment

Discuss the issues you encountered during development and testing. What
problems did you have? What did you have to research and learn on your
own? What kinds of errors did you get? How did you fix them?

What parts of the project did you find challenging? Is there anything that
finally "clicked" for you in the process of working on this project? How well
did the development and testing process go for you?

The thing I felt was most challenging was being unable to run what was needed on my Mac. It gets frustrating not having the tools you need to fully check if your program is working or not. 

I was very lucky to know others in the course and have access to being able to use their machines in order to run and test my program. 

Coding wise, it was just difficult because it was coding blindly, but luckily the example code did provide a good guide and I was able to really understand what was going on.

The only other big issue I recall having was when running the test script, my count was off by one, but after talking to some people in the class, I found out that that was how it was supposed to be. So the time I spent trying to understand why it was off by one wasn't really needed. This is a place I wish I would've immediately emailed the professor or asked because I spent a lot of time trying to figure out where that was occuring. 

## Sources used

https://man7.org/linux/man-pages/man7/signal.7.html <br>
https://www.ibm.com/docs/en/linux-on-systems?topic=hdaa-names-nodes-numbers <br>
