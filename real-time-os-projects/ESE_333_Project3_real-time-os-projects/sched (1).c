//Naafiul Hosain
//ESE333
//PROJ 3
//115107623 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct core{
struct process* p; // pointer to the process currently running on this core
int process_time; // cumulative time this process has been running on the core.
int busy;      // either 0 or 1, 0 if there is no process, 1 if there is process running on the core
};

// virtual computer struct
struct computer
{
	struct core cores[4]; //this computer has 4 cores
	long time;   // computer time in millisecond
};

// struct to store process information
struct process
{
char * process_ID; 
int arrival_time;   // when this process arrives (e.g., being created) 
int service_time;  // the amount of time this process needs to run on a core to finish
int io; // boolean io vlaue (C does not have bool value (c89/90), so use int to demonstrate boolean).
};

// one link in a queue that the scheduler uses to chain process structs, 
// this is a standard struct to create linked list which is generally leveraged to implement queue
struct node 
{
	struct process* p; // the process pointed by this link
	struct node *next; // the pointer to the next link
};


//head for the processes queue
struct node* head;
//tail for the processes queue
struct node* tail;
//head for the arrival queue
struct node* p_head;
//tail for the arrival queue
struct node* pending_tail;
int proc_num; //number of processes in the queue, waiting to be scheduled and not running
int quantum; // the amount of time in milliseconds a process can run on a core before being swapped out
//struct for computer
struct computer computer;

void read_file()
{
	int i,i2;
	FILE* file = fopen("input.txt", "r"); 
    char line[90];
    char name[100];
    char service_time[3];
    char arrival_time[3];
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
        //printf("%s", line); 
        i=0;
        while(line[i]!=' '&&i<90){name[i]=line[i];i++;}
        if(i>90)break;
        name[i]=0;
        i2=++i;
        while(line[i]!=' '&&i<90){service_time[i-i2]=line[i];i++;}
        if(i>90)break;
        service_time[i-i2]=0;
        i2=++i;
        while(line[i]!=' '&&i<90){arrival_time[i-i2]=line[i];i++;}
        if(i>90)break;
        arrival_time[i-i2]=0;
        printf("name: %s, service_time: %d, arrival_time: %d\n",name,atoi(service_time), atoi(arrival_time));

        /* add your code here, you are to creaxute the upcoming processes queue here.
           essentially create a node for each process and chain them in a queue.
           note this queue is *not* the process queue used for round robin scheduling
        */

		struct node *tmpNode = malloc( sizeof( struct node ) );;
		struct process *newProc = malloc( sizeof( struct process ) );

		newProc -> process_ID = malloc( sizeof (50) );
		strcpy(newProc -> process_ID, name);
		newProc -> arrival_time = atoi(arrival_time);
		newProc -> service_time = atoi(service_time);
		int io = 0;
		newProc -> io = io;
	   
	
		//If there is nothing in the p_head, then we must first add.
		if (p_head == NULL) {
			p_head = tmpNode;
			pending_tail = tmpNode;
			tmpNode -> p = newProc;
			tmpNode -> next = NULL;
		} else {

		//if there already exists a queue, simply append to the end.
			tmpNode -> next = NULL;
			pending_tail ->	 next = tmpNode;
			pending_tail = tmpNode;
			tmpNode -> p = newProc;
			tmpNode->next = NULL;

		}
        

        
        
    }
    fclose(file);
    return;
    
}

//this function call simulates one millisecond of time on the computer
void run_one_step() 
{
	int i;
	computer.time++;
	printf("Processing all 4 cores, current Computer time=%lu \n",computer.time);
	for(i=0;i<4;i++)
	{
		if(computer.cores[i].busy)
		{	
			computer.cores[i].p->service_time--; // deduct the remaining service time of the running process by one millisecond
			computer.cores[i].process_time++; // increment the running time for the process by one millisecond in current quantum
			printf("Core[%d]: %s, service_time= %d,been on core for: %d \n",i,computer.cores[i].p->process_ID,computer.cores[i].p->service_time,computer.cores[i].process_time);
			
			
			// you need to swap out or terminate a process if it uses up the current quantum, 
			// or finishes all its service time. The code for this should be located in the main()
			// function, not here.
			// Also if your code is done right, the following warning messages should never print.
			 
			
			if(computer.cores[i].process_time>quantum)
				printf("WARNING: Process on Core[%d] should not run longer than quantum\n",i);
			if(computer.cores[i].p->service_time<0)
				printf("WARNING: Process on core[%d] stayed longer than its service time.\n",i);
		}
	}
}


void run_one_step_p3() 
{
	int rndm,i;
	computer.time++;
	printf("Processing all 4 cores, current Computer time=%lu \n",computer.time);
	for(i=0;i<4;i++)
	{
		if(computer.cores[i].busy)
		{	
		    if(computer.cores[i].p->io==1)
				printf("WARNING: Process on core[%d] has io trigerred, please remove from core, reset io signal and place it back in queue\n",i);
			if(computer.cores[i].p->io==0)
			{
				computer.cores[i].p->service_time--;
				// with 10% probability, generate an io event
				rndm=rand()%10+1;
				if(rndm==10)computer.cores[i].p->io=1;	
			}
			computer.cores[i].process_time++;
			printf("Core[%d]: process %s, service_time= %d,been on core for: %d \n",i,computer.cores[i].p->process_ID,computer.cores[i].p->service_time,computer.cores[i].process_time);
			
			
			// you need to swap out or terminate a process if it uses up the current quantum, has an i/o event; 
			// or finishes all its service time. The code for this should be located in the main()
			// function, not here.
			// Also if your code is done right, the following warning messages should never print.
			
			if(computer.cores[i].process_time>quantum)
				printf("WARNING: Process on Core[%d] should not run longer than quantum\n",i);
			if(computer.cores[i].p->service_time<0)
				printf("WARNING: Process on core[%d] stayed longer than its service time.\n",i);
		}
	}
}


//NOTE: you must free struct node after taking a link off the round robin queue, and scheduling the respective 
// process to run on the core. Make sure you free the struct node to avoid memory leak.
void sched_proc(struct process*p,int core_id)
{
	if(computer.cores[core_id].busy==0)
	{
		printf("Process[%s] with service_time %d has been added to core %d\n",p->process_ID,p->service_time,core_id);
		computer.cores[core_id].busy=1;
		computer.cores[core_id].p=p;
		computer.cores[core_id].process_time=0;
		proc_num --; // xx added accounting of #proc
	}
	else printf("ERROR: must call delete_p to remove current process before adding another to the core.\n");
}

// This handles removing a process from a core, and either discarding the process if its service_time is <=0 
// or adding it to the back of the round robin queue

void delete_p(int core_id)
{
	printf("Process[%s] at core %d has been removed from core with remaining service_time=%d\n",
	computer.cores[core_id].p->process_ID,core_id,computer.cores[core_id].p->service_time);
	
	// if the process has finished all its service time, terminate and clean up
	if(computer.cores[core_id].p->service_time<=0)
	{
		computer.cores[core_id].busy=0;
		// free up allocated memory for process ID and struct upon termination of a process
		free(computer.cores[core_id].p->process_ID);
		free(computer.cores[core_id].p);
		computer.cores[core_id].process_time=0;
	}
	// the process needs to run for more time, put it back into the queue for future scheduling
	else
	{
		computer.cores[core_id].process_time=0;
		// reinsert back to the queue
		if(tail==NULL)
		{
			// in case queue is empty, i.e. all nodes struct were freed and there are no processes in the queue, this will become the first one
			tail=head=malloc(sizeof(struct node)); 
			head->p=computer.cores[core_id].p;
			head->next=NULL; 
			proc_num++;
			computer.cores[core_id].busy=0;
		}
		else
		{ 
			
			tail->next = malloc(sizeof(struct node));
			tail=tail->next;
			tail->p=computer.cores[core_id].p;
			tail->next=NULL;
			proc_num++;
			computer.cores[core_id].busy=0;
			
			
		}
		
	}

}

// a demo running 4 processes until they're finished. The scheduling is done explicitly, not using
// a scheduling algorithm. This is just to demonstrate how processes will be scheduled. In main() 
// you need to write a generic scheduling algorithm for arbitrary number of processes.
void demo()
{
	int i;
	struct process *p0,*p1,*p2,*p3;
	p0=malloc(sizeof(struct process));
	p1=malloc(sizeof(struct process));
	p2=malloc(sizeof(struct process));
	p3=malloc(sizeof(struct process));
	
	p0->process_ID=malloc(sizeof(50));//you can assume process ID will never exceed 50 characters
	p1->process_ID=malloc(sizeof(50));
	p2->process_ID=malloc(sizeof(50));
	p3->process_ID=malloc(sizeof(50));

	strcpy(p0->process_ID,"first");
	strcpy(p1->process_ID,"Second");
	strcpy(p2->process_ID,"Third");
	strcpy(p3->process_ID,"Fourth");

	//assign arrival time
	p0->arrival_time=0;
	p1->arrival_time=0;
	p2->arrival_time=0;
	p3->arrival_time=0;

	//assign service time
	p0->service_time=16;
	p1->service_time=17;
	p2->service_time=19;
	p3->service_time=21;

	// we will skip queue construction here because it's just 4 processes.
	// you must use the round robin queue for the scheduling algorithm for generic cases where many processes
	// exist and may need more than one quantum to finish
	
	
	// xx 4 processes are waiting to be scheduled. No queue is built in demo for simplicity.
	// in your generic algorithm, you should create actual queues, and proc_num should be the number of processes whose
	// arrival time has come, and are waiting in the round robin queue to be scheduled.
	proc_num=4; 
	
	
	//schedule process to each core
	sched_proc(p0,0);
	sched_proc(p1,1);
	sched_proc(p2,2);
	sched_proc(p3,3);

	for(i=0;i<16;i++)run_one_step();
	delete_p(0);
	run_one_step();
	delete_p(1);
	run_one_step();
	run_one_step();
	delete_p(2);
	run_one_step();
	delete_p(3);
	sched_proc(head->p,0);
	
	//NOTE: you must free struct node after scheduling the process. The demo code is not doing it here
	// for simplification, but you have to do it in your code or you will have memory leakage
	
	//head==tail since it was the only one added now to remove it we just make pointer pointing to NULL
	head=NULL;
	tail=NULL;
	run_one_step();
	delete_p(0);
	printf("DONE\n");
}

void init()
{
	quantum=20; //make note
	head=tail=NULL;
	p_head = pending_tail = NULL;
}

int main()
{
	init();
	
	printf("\t*******Reading Input*******\n");
	read_file(); 
	
	
	/* your code goes here for part2. In part 2, you create one node for each process, and put them on an 
	 * 'upcoming process' queue first. Then your code calls run_one_step(), for each process whose arrival time
	 * has come, take the node off the 'upcoming process' queue, and place it on round robin queue. For each
	 * process that's selected to run on a core, take the node off round robin queue.
	 * 
	 * Repeat run_one_step() until all processes finish. Please handle memory allocation/deallocation properly so there's no leak
	 */

	
	printf("\t**ESE 333: Part 2 CODE **\n");
	// using a whule loop we make sure there are still processes in the pending queue (Have not arrived yet) or the computer is still in busy mode.
	//If  1 is busy, then 0 is waiting
	while ((p_head != NULL) || computer.cores[0].busy == 1 || computer.cores[1].busy == 1 || computer.cores[2].busy == 1 || computer.cores[3].busy == 1) {
		
		//Look at the upcoming processes' queue and check if the arrival time has passed. If not, continue on.
		//If the process time has arrived, remove it from the arrival queue and add it to the process queue
		while (p_head != NULL && p_head -> p -> arrival_time <= computer.time) {
			
			//printf("Computer Time is: %ld\n", computer.time);
			printf(" Process found on: %s\n",p_head -> p -> process_ID);
			proc_num++;
			
			// first add the process to the upcoming process queue
			// Check if this is the first entry into the process queue
			if (head == NULL) {
				head = p_head;
				tail = p_head;
				p_head = p_head -> next;
				
				head -> next = NULL;
			} else {
				// in the case, this  is not the first entry, simply add to the tail
				tail -> next = p_head;
				tail = p_head;
				
				
				// If the head can move forward, move forward.
				// Otherwise, set the queue to null and exit the checking loop: there is nothing more to look for.
				// Also, without the break, we risk encountering segmentation faults.
				if (p_head -> next != NULL) {
					p_head = p_head -> next;
					tail -> next = NULL;
				} else {
					p_head = NULL;
					break;
					
				}
			} 			
		}

		// Now that we've added everything required at the current time, let's focus on round robin scheduling.

		// look for the quantum time of the process.

		for (int i = 0; i < 4; i++) {

			
			//if there exists a currently running process, check the quantum time of the process. If it is above our set quantum, remove the process.
			//Also check if the process itself is finished. If it is finished, remove the process.
			if (computer.cores[i].busy == 1 && (computer.cores[i].process_time >= quantum || computer.cores[i].p->service_time == 0)) {
				printf("Removed process from core %d \n", i);
				delete_p(i);
			}

			//Once we have cleared everything, check to see if it is busy. If it is not busy, then add the process from head
			if (computer.cores[i].busy == 0) {
				if (head != NULL) {
					//Add the process from the head, then remove it off the process list
					sched_proc(head->p, i);
					if (head -> next != NULL) {
						struct node* tmp = head;
						head = head -> next;

						free(tmp);
					} else {
						head = NULL;
						tail = NULL;
					}
					
				}
			}

		}


		run_one_step();
	}
	



	
	
	
	
	/* After part 2 is done, you clean up everything, e.g., freeing up all memory allocated,
	 * reset queues to empty etc.
	 * Then restart for part 3: read input file for all processes, initialize queues,
	 *  run processes using run_one_step_p3() so random i/o event can happen at each step on each core, 
	 *  until all processes finish. Remember to clean up again at the end!
	 */


	

	printf("\t**ESE 333 Part 3**\n");
	init();
	read_file();
	computer.time = 0;

//Note I'm not very confident about my part 3 and its execution. 



	//when there are still processes in the pending queue (mean it has not arrived yet) or the computer is in busy mode, then run this loop
	//If 1 is busy then 0 is waiting
	
	while ((p_head != NULL) || computer.cores[0].busy == 1 || computer.cores[1].busy == 1 || computer.cores[2].busy == 1 || computer.cores[3].busy == 1) {
		
		//Look at the upcoming processes' queue and check if the arrival time has passed. If not, continue on.
		//If the process time has arrived, remove it from the arrival queue and add it to the process queue
		while (p_head != NULL && p_head -> p -> arrival_time <= computer.time) {
			
			//printf("ComputerTime is: %ld\n", computer_time);
			printf("Found Process: %s\n",p_head -> p -> process_ID);
			proc_num++;
			
			
			//Add process and check If this is the first entry into the process queue
			if (head == NULL) {
				head = p_head;
				tail = p_head;
				p_head = p_head -> next;
				
				head -> next = NULL;
			} else {
				//Otherwise, if it is not the first entry, simply add to the tail
				tail -> next = p_head;
				tail = p_head;
				p_head->p->io = 0;
				
				
				//If the head can move forward, move forward.
				//Otherwise, set the queue to null, and then leave the checking loop: there is no more to look for
				
				if (p_head -> next != NULL) {
					p_head = p_head -> next;
					tail -> next = NULL;
				} else {
					p_head = NULL;
					break;
					
				}
			} 			
		}

		// round robin
	
		//look for the quantum time of the process. 


		for (int i = 0; i < 4; i++) {

// first there's a currently running process, check its quantum time. If it  > quantum, delete the process.
// Also, check if the process has finished. If it has, remove the process.
// Additionally, check for any IO events. If there have been any, remove the process.
			if (computer.cores[i].busy == 1 && (computer.cores[i].process_time >= quantum || computer.cores[i].p->service_time == 0 || computer.cores[i].p->io == 1)) {
				
				if (computer.cores[i].p->io == 1) {
					computer.cores[i].p->io = 0;
					printf("Event found: There has been an IO event on core %d\n", i);
				}
				
				delete_p(i);
				printf("delete process from core %d \n", i);
			}

			// once you clear
			if (computer.cores[i].busy == 0) {
				if (head != NULL) {
					//Add the process from the head, then remove it off the process list
					sched_proc(head->p, i);
					if (head -> next != NULL) {
						struct node* tmp = head;
						head = head -> next;

						free(tmp); //allocate
					} else {
						head = NULL;
						tail = NULL;
					}
					
				}
			}

		}


		run_one_step_p3();
	}
		
	
	
	printf("\t*******completed*******\n");
	return 0;
}

