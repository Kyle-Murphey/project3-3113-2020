# Kyle Murphey, Project 3 (FINAL)

<ins>Compile:</ins> make all or gcc -c project3.c -o project3

<ins>Run:</ins> ./project3 q p N ALGO

<ins>Bugs:</ins> One unused variable (process->pNum). It was used to keep track of the processes when debugging. 

I assume that all input will be correctly formatted and the files as well.

**main():**
   * reads the CL input and sends the params to the user specified alorithm (FIRSTFIT).
   * runs initial checks on CL input.
   * sets up the processes via createProcs().
    
**firstfit():**
   * finds the first spot available for allocation
   * starts at the beginning of mem, and travels linearly to find the next open spot
   * places node in the first spot big enough
   * runs 'quantum' number of instructions per process
   * keeps track of location in instruction file

**firstfitRR():**
   * loops through all the processes in a Round-Robin fashion
   * sends each incomplete process the the firstfit() algorithm
   * keeps circling through processes until each one is done
   
**isComplete():**
   * check to see if every process is done
   
**setHeads():**
   * set the head of the ddlist for the processes
   
**closeProcs():**
   * closes the processes
   * closes the files
   * calls deleteNodes()
   
**createProcs():**
   * creates pNum amount of processes, connected the last to the head
   * opens the instruction files for each process

**getName():**
   * get the process name

**release():**
   * search though the dllist until node is found and removed
   
**list():**
   * list the available memory slots
   * list the assigned slots

**find():**
   * find a specified node

**deleteNodes():**
   * delete the dllist and free the memory


_Sources:_
   - Linux man pages
   - https://stackoverflow.com/questions/10669673/saving-off-the-current-read-position-so-i-can-seek-to-it-later
   
## Choosing the Quantum
> _The quantum, or "time quantum", is the max run time for each process per cycle.
> This allotted time can affect run time in different ways, depending on the number of
> processes and how much time they need. Imagine you have one very large process at the 
> beginning of the ready queue and 6 smaller processes following it. If the time quantum is
> large, then the 6 following processes will be waiting longer than they should be (especially if
> their runtimes are less than the quantum). If we shrink down the quantum size, it
> will context switch out of the bigger process and onto the smaller ones. This will result in
> a lot lower average waiting times for all of the processes. However, if we reverse the order
> of the processes and place the smaller ones first, a large time quantum will work perfectly.
> All of the processes will complete before it even reaches the large process, allowing it 
> to run continuously. If we shrink the quantum down again, it will not affect the wait time
> in this scenario. But let's picture 10 processes all of equal length: how would the quantum 
> affect them? If the quantum is too small, then there will be a lot of context switches 
> and that will slow down the processes. Ideally, you want the majority of your processes to 
> take less time than the allowed amount given by the quantum. This will allow for a shorter average
> waiting time and a lot less context switches and an overall increase in efficiency._