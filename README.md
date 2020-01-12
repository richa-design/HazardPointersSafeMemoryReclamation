# Hazard Pointers Safe Memory Reclamation Scheme
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;One of the major concerns for Lock-free data structures is the reclamation of memory occupied by removed nodes. Unmanaged languages that do not have automatic garbage collection support, Safe Memory Reclamation (SMR) techniques are designed to ensures that memory of removed nodes is freed (for reuse or returned to the OS) only when no other thread is having reference to the removed node along with providing lock-free progress guaranteed by the data structure. C and C++ are examples of such un managed programming languages. These languages don’t have inbuilt capability of garbage collection.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Hazard Pointers is a methodology for safe-memory reclamation and ABA problem prevention for lock-free data structures. This methodology links a set of hazard pointers with each thread that intends to access the lock-free object. Hazard pointer is a single-writer, multi-reader pointer that can be owned by at most one thread. Value of each hazard pointer can be set only by the owner of that pointer, while other threads can only read the value of pointer. The number of hazard pointers depends on the algorithm used for the concurrent data structures. We have implemented Hazard pointers for two data structures, lock-free Stack and lock-free Queue. So, the number of hazard pointers for stack is one while two for queue. Hazard Pointers have initial value of null. The owner thread sets its value to the node’s address (reference) in order to indicate to concurrent threads that it may later use that reference to access the contents of the node without any further validation to determine that the reference to the node is still valid or not. This indication prevents other threads from reclaiming that node until no hazard pointer holds this reference. This reference is also known as hazardous reference because freeing a node that might still be referenced by other threads is hazardous.


## Compilation and Execution
Following are the details of source and header files:
1. /HazardPointers/src/main.cpp: File which is responsible to take inputs from user. This file requires two inputs: 1. Thread Count 2. Iteration Count.

2. /HazardPointers/inc/TestThread.h: Header file which is responsible for execution of each thread. Each thread executes the run method in this file.

3. /HazardPointers/inc/node.h: Header file which defines the node data structure.

4. /HazardPointers/inc/common.h: This file is used to include common header file. There are two major macros in this file: 
  A. Enable_Hazard_Pointer: This macro must me defined in order to run Hazard Pointers scheme for memory reclamation. If not defined, the program will run the data structures without any SMR scheme.  
	B. ENABLE_STACK_OPER: This macro must be defined in order to select Stack as operating data structure on which relamation scheme is applied. If not defined, by default, program will pick Queue as default data structure.

5. /HazardPointers/inc/LockFreeQueue.h: C++ implementation of Lock Free Queue.

6. /HazardPointers/inc/LockFreeStack.h: C++ implementation of Lock Free Stack.

7. /HazardPointers/inc/HazardPointer.h: Hazard Pointers scheme is implemented inside this header file.

How to compile and execute the project on UNIX system:
1. Goto /HazardPointers/src/ directory and type following commnd in shell:
   ```
   g++ main.cpp -o main  -std=c++11 -lpthread
   ```
   Above command will create an executable named main inside source directory.

2. To simply run the program, type following command:
   ```
   ./main <thread count> <iterations> 
   ```
   
3. If you want to run the program with memory profiler, type following command:
   ```
   valgrind --tool=massif ./main <thread count> <iterations> 
   ```
   Tool, after completing the execution will generate a report file with name which will appear something like massif.out.<b>id</b>. <b>id</b> is printed every time tool exits the execution.

4. To analyze the report, type following command:
   ```
   ms_print <report name>
   ```


