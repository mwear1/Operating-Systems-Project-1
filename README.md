# Operating-Systems-Project-1
Implementation of producer-consumer program in a Linux environment.
# Description
This project is a two-process Producer-Consumer demonstration using POSIX shared memory and named semaphores. The producer writes pairs of random integers
into a 256-byte shared buffer. The consumer reads these numbers from the buffer and prints them.
It does a cycle of production and consumption ten times before unlinking and closing.
# Installation/Execution
All required files are provided here.
The files used during execution are:
```
producer.cpp
consumer.cpp
producerconsumer.hpp
test.txt
```
# Dependencies
The following libraries are used in this program:
```
<stdio.h>
<stdlib.h>
<string.h>
<iostream>
<cstring>
<pthread.h>
<semaphore.h>
<random>
<unistd.h>
<fcntl.h>
<sys/shm.h>
<sys/mman.h>
```
Hardware Versions used:  
Visual Studio Code v1.105.1  
OS: Linux x64 6.14.0-27-generic snap  
# Compilation
Guid for compilation:  
  1. Load Visual Studio Code in a Linux/UNIX environment (can be done natively, remotely, or through virtualization)
  2. Open the terminal
  3. Enter the datapath to the folder
  4. Run the following commands (they are separate files):
```
$ g++ producer.cpp -pthread -lrt -o producer
$ g++ consumer.cpp -pthread -lrt -o consumer
$ ./producer & ./consumer &
```
# Examples
Below are screenshots of the program in execution:
1. Compiling producer program
<img width="1920" height="955" alt="Screenshot from 2025-10-19 18-27-21" src="https://github.com/user-attachments/assets/3631dd51-839e-4a96-b4b6-dd60eae17ae1" />
2. Compiling Consumer program
<img width="1920" height="955" alt="Screenshot from 2025-10-19 18-27-41" src="https://github.com/user-attachments/assets/45a5bec4-643b-4f71-8eb1-98b340b9e8a5" />

3. Both programs in execution
<img width="1920" height="955" alt="Screenshot from 2025-10-19 18-29-13" src="https://github.com/user-attachments/assets/0e95291e-9dbb-44d9-a6ad-738e1724cb8a" />
