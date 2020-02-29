/* File: super_new_scheduler.c
 * Student: Alex Cooke (18406489)
 * Description: The file contains both task() and cpu() functions
 * in addition to main.
 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include "scheduler.h"
#include "ready_queue.h" /*contains Task struct */
#include "fileio.h"

/* * * * * * * * * * * * * * shared global variables * * * * * * * * * * */
Task** ready_queue; /* an array of pointers to Task structs */

pthread_mutex_t mutex; /* acquire a lock on shared data */
pthread_cond_t empty; /* wait for there to be more in the buffer */
pthread_cond_t full; /* wait for there to be less in the buffer */

int occupied; /* number of occupied spots in the ready_queue */
int next_in; /* producer adds next item to here */
int next_out; /* consumers takes next item from here */
int length_of_queue; /* the number of items in the ready_queue */
int task_thread_finished; /* global variable to signal completion of task thread */

int total_num_tasks;
int total_wait_time;
int total_turn_time; /* total turnaround time */

FILE* task_file;
FILE* simulation_log;

int main( int argc, char* argv[] )
{
    /* * * * * * * * * * * * * * variable declarations * * * * * * * * * * */
      double av_wait_time;
      double av_turn_time;
      int i;

      pthread_t task_thread;
      pthread_t CPU1;
      pthread_t CPU2;
      pthread_t CPU3;

    /* * * * * * * * * * * * * * variable initialisation * * * * * * * * * * */
      task_file = NULL;
      simulation_log = NULL;
      pthread_mutex_init(&mutex, NULL); /* initialise the shared mutex lock */
      pthread_cond_init(&empty,NULL); /* initialise conditional variables */
      pthread_cond_init(&full,NULL);

      occupied = 0;
      next_in = 0;
      next_out = 0;
      task_thread_finished = 0;

      total_num_tasks = 0;
      total_wait_time = 0;
      total_turn_time = 0;

      av_wait_time = 0;
      av_turn_time = 0;

      /* * * * * * * * * * * * * * begin * * * * * * * * * * */

      if ( argc == 3 ) /* Check for correct number of params */
      {
          length_of_queue = atoi(argv[2]);
          ready_queue = (Task**) malloc( sizeof( Task* ) * length_of_queue );
          for( i = 0; i < length_of_queue; i++ )
          {
              ready_queue[i] = (Task*) malloc( sizeof(Task) );
          }

          /* Open files */
          task_file = fopen( argv[1], "r" );
          simulation_log = fopen( "simulation_log.txt", "w" );

          /* Check that both files were succesfully opened */
          if ( (task_file != NULL) && (simulation_log != NULL) )
          {
            /* create the threads */
            pthread_create(&task_thread,NULL,task,NULL);
            pthread_create(&CPU1,NULL,cpu,"CPU1");
            pthread_create(&CPU2,NULL,cpu,"CPU2");
            pthread_create(&CPU3,NULL,cpu,"CPU3");

            /* wait for all the threads to finish */
            pthread_join(task_thread, NULL);
            pthread_join(CPU1, NULL);
            pthread_join(CPU2, NULL);
            pthread_join(CPU3, NULL);

            /* calculate average waiting and turnaround time */
            av_wait_time = ((double)total_wait_time) / ((double)total_num_tasks);
            av_turn_time = ((double)total_turn_time) / ((double)(total_num_tasks));
            writeFinalSummary(simulation_log, total_num_tasks, av_wait_time, av_turn_time);
            
            /* deallocate memory */
            for( i = 0; i < length_of_queue; i++ )
            {
                free(ready_queue[i]);
            }

            free(ready_queue);
            
          }
          else /* Error opening input and output files if either is NULL */
          {
              printf( "Error opening files\n" );
          }
          fclose( task_file );
          fclose( simulation_log );
      }
      else
      {
          printf( "Incorrect number of command-line parameters\n" );
      }

      return 0;
}

/*
This is the entry function for task thread.
This thread is responsible for reading items from task_file.txt, and placing
them in the ready_queue, if there's space to do so. If not, the thread will
wait until one of the CPU threads consume an item from the read_queue.
The task thread terminates when there are no more items to be read.
*/
void* task(void* param)
{
    Task* task;
    Task* taskTwo;
    int more_items_to_be_read;
    more_items_to_be_read = 1;
    task = NULL;

    while(more_items_to_be_read) /* e.g. while(1) */
    {
        pthread_mutex_lock(&mutex); /* lock the ready_queue */
        while ( occupied >= length_of_queue ) /* If the ready_queue is full */
        {
            /* Wait for an empty spot in the ready_queue (i.e. wait for
            consumer to remove an item). Release the mutex on the ready-queue
            so that can happen. */
            pthread_cond_wait(&full, &mutex);
        }

        /* CRITICAL SECTION: CURRENTLY ONLY PRODUCING ONE TASK AT A TIME */
        task = (Task*) malloc(sizeof(Task));

        /* getItem(...) found in fileio.c. Returns a pointer to next Task in task_file.txt */
        more_items_to_be_read = getItem(task, task_file);

        /* If next item is not EOF, put it in the queue */
        if ( more_items_to_be_read != 0 )
        {
            free(ready_queue[next_in]); /* free element to be overwritten */
            ready_queue[next_in] = task;
            next_in++;
            /* If last index in ready_queue, then go back to the beginning,
            otherwise do nothing. Increment number of occupied spots
            in the ready_queue */
            next_in %= length_of_queue;
            occupied++;
            total_num_tasks++;

            /* Write to simulation_log.txt */
            writeArrivalTask(task, simulation_log);
            /* Signal that there is an item in the ready_queue ready to
            be consumed. This breaks the consumer out of its loop */
            pthread_cond_signal(&empty);
        }

        /* if there's space for a second task in the ready_queue, then add it */
        if( occupied < length_of_queue )
        {
            taskTwo = (Task*) malloc(sizeof(Task));

            /* getItem(...) found in fileio.c. Returns a pointer to next Task in task_file.txt */
            more_items_to_be_read = getItem(taskTwo, task_file);

            /* If next item is not EOF, put it in the queue */
            if ( more_items_to_be_read != 0 )
            {
                free(ready_queue[next_in]); /* free element to be overwritten */
                ready_queue[next_in] = taskTwo;
                next_in++;
                /* If last index in ready_queue, then go back to the beginning,
                otherwise do nothing. Increment number of occupied spots
                in the ready_queue */
                next_in %= length_of_queue;
                occupied++;
                total_num_tasks++;

                /* Write to simulation_log.txt */
                writeArrivalTask(taskTwo, simulation_log);
                /* Signal that there is an item in the ready_queue ready to
                be consumed. This breaks the consumer out of its loop */
                pthread_cond_signal(&empty);
            }
        }
        pthread_mutex_unlock(&mutex); /* release the lock on the ready_queue */
    }

    /* no more items to be read. Thread task should terminate */
    pthread_cond_signal(&empty);
    free(task); /* free data still remaining at task pointers */
    free(taskTwo);
    pthread_mutex_lock(&mutex); /* lock the simulation_log */
    writeTaskThreadComplete(simulation_log, total_num_tasks);
    task_thread_finished = 1; /* Set flag probed by CPU threads */
    pthread_mutex_unlock(&mutex); /* release the lock on the simulation_log */
    pthread_exit(0);
}
/*
This is the entry function for the CPU threads.
These threads are responsible for consuming items from ready_queue.
If the ready_queue is empty, then the CPU thread will wait either until
task thread adds an items to the ready_queue, or task thread terminates.
*/
void* cpu(void* param)
{
    Task* task;
    int num_tasks_served_by_this_cpu;
    time_t service_time;
    time_t completion_time;
    task = NULL;

    num_tasks_served_by_this_cpu = 0;

    while(1)
    {
        pthread_mutex_lock(&mutex); /* lock the ready_queue */
        while ( occupied <= 0 ) /* If the ready_queue is empty */
        {
            /* If task thread has terminated,  and no more items in the queue
               then terminate */
            if( (task_thread_finished == 1) && (occupied <= 0) )
            {
                pthread_cond_signal(&empty);
                writeCpuThreadComplete(simulation_log, param, num_tasks_served_by_this_cpu);
                pthread_mutex_unlock(&mutex);
                pthread_exit(0);
            }

            /* Wait for an item to be put into the ready_queue (i.e. wait for
               producer to insert an item). Release the mutex on the
               ready-queue)
            */
            pthread_cond_wait(&empty, &mutex);
        }
        /* CRITICAL SECTION */
        task = ready_queue[next_out++];
        time(&service_time);
        writeCPUServiceTask(simulation_log, task, param, service_time);
        num_tasks_served_by_this_cpu++;
        total_wait_time += (int) (difftime(service_time, task->arrival_time));

        /* sleep for cpu_burst seconds */
        sleep(task->cpu_burst);
        completion_time = service_time + task->cpu_burst;
        writeCPUCompleteTask(simulation_log, task, param, completion_time);
        total_turn_time += (int)(difftime(completion_time, task->arrival_time));

        /* find the next out */
        next_out %= length_of_queue;
        occupied--; /* one less spot occupied in the ready_queue */
        pthread_cond_signal(&full); /* Signal that there's one less thing in the 
        ready_queue (i.e. one more available spot to be written to by task()) */

        pthread_mutex_unlock(&mutex); /* Release the lock on the ready_queue */
    }
}
