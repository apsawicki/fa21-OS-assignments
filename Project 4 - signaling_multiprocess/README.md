# Project-4-Fall21
# Project 4 – Signaling with Multi-Process Programs
**Problem**:
This project requires _a program development_ and an analysis of performance for the solution.

**Summary**: 
The purpose of this project is to have a process and its threads responsible for servicing “signals” sent by other peer processes. The processes and threads must execute concurrently, and performance of all process and thread activities is to be monitored and analyzed.

**Learning Objectives**: 
This project expands on our knowledge and experience of creating applications with multiple processes and multiple threads. The project also introduces signals as communication mechanisms. Synchronization in the form of protected access to shared variables is used.

**Project Description**:
This project is to be implemented using Linux and the signaling system calls available in Linux. A signal is an event that requires attention. They can be thought of as a software version of a hardware interrupt. There are many different signal types and each signal type has a defined action. The action of many of the signal types can be changed by developing a “signal handler” in code to process the receipt of a particular signal type.

There are signals that are considered “synchronous” and signals considered “asynchronous”. A synchronous signal is one that is the result of an action of a particular thread such as a divide by zero error or an addressing error. An asynchronous signal is one that is not related to a particular thread’s activity and can arrive at any time, that is with no specific timing or sequencing. In this project we are dealing with asynchronous signals.

In Linux, asynchronous signals sent to a multi-threaded process are delivered to the process. The decision as to which thread should handle the arriving signal is based upon the *signal mask configuration* of the threads within the process. If more than one thread has not ‘blocked’ the received signal, there is no guarantee as to which thread will actually receive the signal.

You will need to do some research about signals and signal masks. There are a number of system calls related to signals and signal masks that you can use in the implementation of your solution to the project.
 
The **_Program_** (you are developing a single program with multiple processes) for this project requires developing a solution using a *parent process*, two additional signal generating processes and a _signal handling_ *process that contains 5 threads.* 

For the **_Program_**, the parent process creates the additional 3 child processes, controls execution time of the child processes and waits for their completion. In addition, the parent process behaves as a signal generator. The *_Program_* is to use two signals for communication: SIGUSR1 and SIGUSR2. The parent process and each of the two child signal generating processes will execute in a loop, and during each loop repetition will randomly select either SIGUSR1 or SIGUSR2 to send to the signal receiving process. Each repetition through the signal generating loop will have a random time delay ranging between .01 second and .1 second before moving to the next repetition. After generating a signal, the process will log a ‘signal sent time’ and ‘signal type’. The time value must have sufficient resolution to distinguish among signals sent by the processes.

The logs of the signal sending processes should be written to a common file. For your analysis, it would be helpful if the log is sorted in time.

Each of the signal handling threads will process only one type of signal, either SIGUSR1 or SIGUSR2 and ignore servicing the other signal type. (Thus, two of the four signal receiving threads are assigned to handle each type of signal). Each of the signal handling threads will also execute in a loop, waiting for its signal to arrive. When the signal arrives, the thread will increment a shared ‘signal received’ counter for that signal type (i.e., a counter globally available to all threads in the process that need access to that counter) and save the time of receipt of the signal and the ID of the receiving thread. Each signal type has its own signal received counter. The thread will then loop waiting for the next signal arrival.

The reporting thread in the receiving process will monitor the receive signal counters and when an aggregate of 16 signals have arrived, it will log the receive times and receiving thread IDs for those signals. The reporting thread should also log the average time between receptions of each signal type during the interval in which the 16 signals are received. This calculation will make use of the time of receipt of each signal type.

Access to the global counters by each thread requires the use of a critical section control. *You are to use a lock for the critical section control.*Use the lock to provide each thread private access to each global counter.

**Results to Report**: 
We are interested in the performance of the threads receiving signals. This means you will need to “stress” the receiving process and its threads to see whether it is able to handle the signal service demand made by the signal senders. You may want to place additional limits on the interval between signals sent in order to cause failures in signal processing.

A. You should run the program for 30 seconds and examine the performance; that is, the number of signals of each type sent and received. Also examine the average time between signal receptions of each type by the reporting thread. Be sure to investigate and discuss any signal losses.     
B. You should run the program for a total of 100,000 signals and look at the total time and counts for the signal types, as well as the average interval between signal receptions of each signal type by the reporting thread. (i.e., report the same information as in part A.)  
C. Since you are logging the times of signals sent by each of the sending processes and the receive times by the threads, you should be able to determine when (time and conditions) the sending demand (intervals) results in signal losses. 

Be sure to include a **test plan** for the program, and document the results of your testing such that you can **_clearly demonstrate_** that the program **_executes correctly_**.

You will develop your program using the supplied GitHub repository. You will submit your program code, testing documents and execution results through Canvas. Compress all of the files (do not include executables) into one file for submission. Use clear naming to help identify the project components that are submitted. Include a link to your GitHub repository as a comment to your Canvas submission.

There will be **weekly deliverables** and they are to be *submitted to Canvas* as well. 

Week 1: 
* Creation of the main process, the 2 signal generator processes, the signal catcher processes and its threads. (one source for signal generators, one source for signal catcher threads).
* Design and implementation of interval clocks
* Design signal masks for the threads 

Week 2: 
* Create shared counters and individual locks for each counter.
* Proper/safe access to send and receive counts
* Design signal catcher routines
* Description of testing and documentation (did they include logs in processes, e.g.).

Week 3 [Complete Submission] 
* Creation of proper functioning signal catcher routines
* Proper use of signal masks to enable/disable signal receptions
* Document Production 
	* Reporter receipt and time stamping of signals and intervals for process execution  
	* Description and analysis/comparison of results from execution – including multiple executions and comparison of data. 
**Final executable and Demo** 

**Grading Rubric** 
* Creation of the main process, the 2 additional signal generator processes, the 4 signal catcher threads and the reporter thread. (1 Pt)
* Design and Implementation of Interval clocks (1 pt)
* Create shared counters and individual locks for each counter (1 pt) 
* Proper/safe access to send and receive counts (1 pt) 
* Description of testing and documentation for signal senders (1 pt) 
* Logging of signal send information at the sender processes (1 pt) 
* Creation of proper functioning signal catcher routines (1 pt) 
* Proper use of signal masks to enable/disable signal receptions (1 pt) 
* Reporter receipt and time stamping of signals and intervals for Threads (1 pt) 
* Description and analysis/comparison of results from execution (1 pt) 
Total 10 Points


