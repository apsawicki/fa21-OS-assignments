# Project-1-Fall21
# *CIS 3207 Project 1*  
## Giorgio’s Discrete Event Simulator  
### Description  
We have been discussing virtualization, concurrency and the sharing of resources by the operating system. In order for the OS to enable the shared use of a single resource, it must provide a way to allocate the resource to a process and then release the resource from the process when the process has finished using it. Since many processes may want to use the resource at any time, there needs to be a mechanism for the OS to organize and order the use of the resource. This can be accomplished by having an ordered list of processes waiting to use the resource, and the OS choosing a process from the list of waiters when the resource becomes available. Then the OS assigns the process to the resource for its work.

A process moves through various states during its lifetime: waiting to use the CPU, executing and crunching numbers using the CPU, waiting for a request for the disk to complete a read or write, waiting for a user to click on something with a mouse, etc. In class, we have begun studying the various states that a process can move through and the relationships between states.
The purpose of this assignment is to gain some understanding of how processes move through a computing system and to put into practice some things that you’ve learned in 2107 and 2168 but might not have had a chance to use in a while.  
![Figure 1](/Project1Figure.jpg)

In this assignment, you’ll create a  [discrete event simulation](https://en.wikipedia.org/wiki/Discrete_event_simulation)  in C that *models* the very simplified computing system pictured below. The diagram shows the flow of a process through the different states during its execution history. It also shows that a process must wait to use a resource if the resource is in use or busy. [Some additional information about discrete event simulation is in  [Supplemental Information: Discrete-Event Simulation Problem](DiscreteEventSimulation.pdf)

When a new job arrives in the system (perhaps because a user double-clicked on an icon to start a program), the job needs to spend some amount of time executing using the CPU. The job then either performs I/O on one of two disks or sends a message to the network and next returns to use the CPU.  Or after using the CPU, the job is finished (terminates). Jobs cycle between using the CPU and I/O devices in this manner until they are finished.  

It’s possible that when a job arrives to use a device, the device is busy servicing another task. In this case, the job must wait in the “device queue” until the device is no longer busy. For this project, all of your device queues are FIFO, however, this is not necessarily the case in real systems.  

For this assignment, we’ll frequently note or record the time of an event; however, we’ll use a “logical clock” and think of things in terms that sound like, “at time 37, job 6 spent 5 units of time at the CPU” rather than “at 2:30:01 PM, job 6 spent 300 microseconds at the CPU”. Time begins at 0 when the simulation begins, and advances as the processes cycle through the devices.  

In the simulation, there are events and device usage. Each of these events is associated with a time in the simulation. For example, a process (task) arrives to use the CPU and there is an associated time of arrival; the task uses the CPU for a period of time; the task finishes using the CPU at an associated time (the time determined by time of arrival plus usage time). Each device behaves in the same way.   

In your simulation, the amount of time that a job spends using the CPU in a single continuous burst of computing, which disk contains the file a job needs, or whether to use the network, and many other properties are *determined at runtime, as random intervals*. To generate random numbers in C, take a look at the Standard C Library functions rand( ) and srand( ) (so that your program doesn’t use the same pseudo-random numbers in each run or program execution).  

### Basic Mechanism  
The system behaves as follows:  
* The system runs from INIT_TIME (usually 0) to FIN_TIME (a specified period).
* New jobs enter the system with an interarrival time that is uniformly distributed between ARRIVE_MIN and ARRIVE_MAX.
* Once a job has finished an interval of processing using the CPU, the probability that it completes and exits the system (instead of next doing a disk read or network send, and then further computation) is QUIT_PROB.
* Once a job has been determined to continue executing (i.e., not terminated), a probability function is used to determine whether it will do disk I/O or use the network. This probability is NETWORK_PROB.
* When a job needs to do disk I/O, it uses the disk that’s the least busy, i.e., the disk whose wait queue is the shortest. (This might seem a bit silly, but we can pretend that each disk has the same information.) If the disk queues are of equal length, choose one of the disks at random.
* When jobs reach some component (CPU, disk1, disk2, or network), if that component is free, the job immediately begins service on that component. If, on the other hand, that component is busy servicing another task, the job must wait in that component’s queue.
* The queue for each system component is FIFO.
* When a job reaches a component (a different job leaves a component, or the component is free, and this job is first in the queue), we need to know how much time the selected job will spend using the component. This is determined randomly, at runtime, for each component arrival. A job is serviced by a component for an interval of time that is uniformly distributed between some minimum and maximum defined for that component. For example, you’ll define: CPU_MIN, CPU_MAX, DISK1_MIN, DISK1_MAX, etc.
* At time FIN_TIME, the entire job simulation terminates. We can ignore the jobs that might be left receiving service or waiting in the queue when FIN_TIME is reached.

### Implementation  
This might all seem to make sense in theory, but how is it implemented? Your program will have 5 non-trivial data structures: one FIFO queue for each component (CPU, disk1, disk2, network), and a priority queue used to store events and drive the system. An event in the priority queue might be something such as “a new job entered the system”, “a disk read completed”, “a job finished at the CPU”, etc. Events should be removed from the priority queue and processed based on (using) the time that the event is to occur. That is, current time is set to the time of the event pulled from the priority queue. The priority queue can be implemented as a sorted list or minheap.  

The simulation will be driven (continue to run) by removing events from the priority queue and acting on the event. Each event that is removed has a task to perform and an event time. The event time associated with an event removed from the priority queue establishes a new ‘current time’ for the simulation. The task associated with the event could be any of those mentioned above such as “a disk read completed”, “ job arrived at CPU”, etc.  

When your program begins, after some initialization steps, the program will add to the priority queue two tasks: the arrival time of the first job, and the time the simulation should finish. Suppose that these are times 0 and 100000 respectively. Priority (or order of events) in the queue is determined by time of the event (earliest time is highest priority and is at the front of the queue). The priority queue would look like:  
<pre>
<b> when       what</b>  
   0       job1 arrives  
  100000   simulation finished  
  </pre>
  
Until we’re finished, we start to remove and process events from the priority event queue. First, we remove the event “job1 arrives” because it has the lowest time (and, therefore, the highest priority). In order to process this JOB_ARRIVAL_EVENT we:  

1. set the current time of the simulation to 0, i.e., the time of the event we just removed from the queue  
2. <b>determine the arrival time for the next job to enter the system</b> and add it as a new event to the priority queue (this is creating a new job arrival, and keeps the simulation going).  
3. send job1 to the CPU (it is a new job arrival and newly started programs need to start by executing on the CPU). 

To <b> determine the time of the next arrival </b>, we generate a random integer between ARRIVE_MIN and ARRIVE_MAX and *add it* to the current time. This will be the time of the 2nd arrival. As an example, we end up with a time of 15 (units) that we add to the current time of 0.

As stated, whenever a ‘new job arrival’ event is removed from the priority queue, another new job arrival is created and entered in the priority queue.  

As we pull the first job event from the priority queue, we also need to add this job1 to the CPU (because it is a new job arrival). Because the CPU is initially idle, it can start work on job1 right away. We then add another event to the priority queue. This is the time at which job1 will finish at the CPU. To determine the CPU finish time, we generate a random integer between CPU_MIN and CPU_MAX and add it to the current time. Suppose that we end up with a CPU usage time of 25 (units). Remember, current time is 0 and this is the time associated with Job1 when the event was pulled from the priority queue.  

The event queue is now:
<pre>
<b> when       what</b>  
  15        job2 arrives
  25        job1 finishes at CPU
 100000     simulation finished
 </pre>

We proceed the same way until we remove a SIMULATION_FINISHED event from the queue. The main loop of your program will then be something like:  
<pre>while the priority queue is not empty (it shouldn’t ever be)  
     and the simulation isn’t finished:  
        read an event from the queue  
            handle the event  
</pre>
     
Each event in the event queue will be of a given type. It seems simplest to store these as named constants, e.g., something like CPU_FINISHED=1, DISK1_FINISHED=2, etc., and to *write a handler function for each possible event type*. We’ve just described what the handler for a job arrival event might do. As another example, what should a “disk finished handler” do? Remove the job from the disk, (and according to the diagram) return the job to the CPU.  if the CPU is free, we can perform the ‘start CPU usage’ function that you will develop. If the CPU is not free, add it to the CPU’s queue just as we did previously for a CPU arrival. We should also look at the disk’s queue (because the disk is now free). If the disk’s queue isn’t empty, we need to remove a job from its input queue, determine how long the job will require using the disk, create a new “disk finished” event and add it to the event queue. We set the disk as in use (not free).  

The network finished handler will behave in a similar way to the ‘disk finished’ handler when the NETWORK_FINISHED event occurs.  

### Running Your Simulator  

The program will read from a text configuration file the following values:
* a SEED for the random number generator
* INIT_TIME
* FIN_TIME
* ARRIVE_MIN
* ARRIVE_MAX
* QUIT_PROB
* NETWORK_PROB
* CPU_MIN
* CPU_MAX
* DISK1_MIN
* DISK1_MAX
* DISK2_MIN
* DISK2_MAX
* NETWORK_MIN
* NETWORK_MAX

The format of the file is up to you, but it could be something as simple as:  
INIT_TIME 0  
FIN_TIME 10000. 

…
### Results
#### log  
Your program should write to a log file the values of each of the constants listed above as well as each significant event (e.g., arrival of a new job into the system, the completion of a job at a component, the termination of the simulation, along with the time of the event). That is, log each event processed.  

#### statistics. 
Calculate and print:  
* The average and the maximum size of each queue.  
* The utilization of each server (component). This would be: time_the_server_is_busy/total_time where total_time = FIN_TIME-INIT_TIME.  
* The throughput (number of jobs completed per unit of time) of each server.  

Run the program a number of times with different values for the parameters and random seed. Examine how the utilizations relate to queue sizes. If for a given choice of parameters by changing the random seed you obtain utilization and size values that are stable (i.e., they do don’t change much (maybe 10%)), then you have a good simulation.  
Your program should process a reasonable number of jobs, at least one thousand.  

As part of the homework submit a document, README.txt, from two to three double-spaced pages plus (maybe include) diagrams, describing your program. Your description should be written, as though addressed to a technical manager that needs to understand what you have done, the alternatives that you had to choose in design, why you made the choices you made, and how you tested your program.  

Include also a second document, RUNS.txt, describing the data you have used to test your program and what you have learned from executing and testing. You should choose reasonable values for the inter-arrival times (interval between new job arrivals) and for the CPU service times. For simplicity, choose a QUIT_PROB defaulted to 0.2, and use a service time at the disks equal to the service time of real disks (i.e., what is the average time to access and read or write a block of disk?). You can use a value of 0.3 for NETWORK_PROB, indicating that 70% of the time a job leaving the CPU will do disk I/O and 30% of the time the task will perform a network send. If you can, determine what is the smallest reasonable inter-arrival time (and for that matter, how would we even go about deciding on what reasonable would be)?  
![Figure 2](/Project1Rubric.jpg)
