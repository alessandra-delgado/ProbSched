// Where main simulation action will take: generating processes, and user interface is called in

// 1 - Comunicating with the user:
// a) Ask if user wants to generate from file, or if random (with limit of process amount or just infinite until program ends)
//      - must return process vector to here
//      -> if processes keep being added to the list, the scheduler will have to be implemented in a way that allows both ways to recieve processes
//          ? use global variables for this matter ^^^ ?
// b) Ask which algorithm the user wants to use to simulate the cpu scheduling
//      throw in the ready queue, or update the ready queue along with the scheduling

// 2 - CPU Scheduling starts
//      - algorithm runs
//      - should the stats be displayed during 


// have two separate functions in this file that take care of the different types of scheduling (run time or predefined)