#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include "ready_queue.h"

/* getItem(...) performs a read operation, and points task pointer at a newly
   read task. An int is returned to indicate either success (1), or 
   failure (0) [i.e. end of file]
*/
int getItem(Task* task, FILE* task_file)
{
    int nRead, new_task_ID, new_cpu_burst; /*new_hrs, new_mins, new_secs;*/
    time_t now;
    time(&now);
    nRead = 0;

    nRead = fscanf( task_file, "%d %d\n", &new_task_ID, &new_cpu_burst );

    if ( nRead != EOF )
    {
        task->task_ID = new_task_ID;
        task->cpu_burst = new_cpu_burst;
        task->arrival_time = now;
    }
    else
    {
        /* EOF occured */
        return 0;
    }
    return 1;
}

/* Write the arrival of a task to the simulation_log.txt */
void writeArrivalTask(Task* task, FILE* simulation_log)
{
    int hours, minutes, seconds;
    struct tm* local;
    local = localtime(&(task->arrival_time));
    hours = local->tm_hour;
    minutes = local->tm_min;
    seconds = local->tm_sec;

    fprintf(simulation_log, "%d: %d\nArrival time: %02d:%02d:%02d\n\n", task->task_ID, task->cpu_burst, hours, minutes, seconds);
}

/* Write service information to the simulation log */
void writeCPUServiceTask(FILE* simulation_log, Task* task, char* CPU_num, time_t service_time)
{
    int service_hours, service_minutes, service_seconds;
    int arrival_hours, arrival_minutes, arrival_seconds;
    struct tm* formatServiceTime;
    struct tm* formatArrivalTime;

    formatArrivalTime = localtime(&(task->arrival_time));
    arrival_hours = formatArrivalTime->tm_hour;
    arrival_minutes = formatArrivalTime->tm_min;
    arrival_seconds = formatArrivalTime->tm_sec;

    formatServiceTime = localtime(&service_time);
    service_hours = formatServiceTime->tm_hour;
    service_minutes = formatServiceTime->tm_min;
    service_seconds = formatServiceTime->tm_sec;

    fprintf(simulation_log, "Statistics for %s:\nTask %d\nArrival time: %02d:%02d:%02d\nService time: %02d:%02d:%02d\n\n", CPU_num, task->task_ID, arrival_hours, arrival_minutes, arrival_seconds, service_hours, service_minutes, service_seconds);
}

/* Write single task completion information to the simulation_log.txt */
void writeCPUCompleteTask(FILE* simulation_log, Task* task, char* CPU_num, time_t completion_time)
{
    int completion_hours, completion_minutes, completion_seconds;
    int arrival_hours, arrival_minutes, arrival_seconds;
    struct tm* formatcompletionTime;
    struct tm* formatArrivalTime;

    formatArrivalTime = localtime(&(task->arrival_time));
    arrival_hours = formatArrivalTime->tm_hour;
    arrival_minutes = formatArrivalTime->tm_min;
    arrival_seconds = formatArrivalTime->tm_sec;

    formatcompletionTime = localtime(&completion_time);
    completion_hours = formatcompletionTime->tm_hour;
    completion_minutes = formatcompletionTime->tm_min;
    completion_seconds = formatcompletionTime->tm_sec;

    fprintf(simulation_log, "Statistics for %s:\nTask %d\nArrival time: %02d:%02d:%02d\ncompletion time: %02d:%02d:%02d\n\n", CPU_num, task->task_ID, arrival_hours, arrival_minutes, arrival_seconds, completion_hours, completion_minutes, completion_seconds);
}

/* Write task thread completion to the simulation_log.txt */
void writeTaskThreadComplete(FILE* simulation_log, int num_tasks)
{
    time_t now;
    int completion_hours, completion_minutes, completion_seconds;
    struct tm* formatcompletionTime;

    time(&now);
    formatcompletionTime = localtime(&now);
    completion_hours = formatcompletionTime->tm_hour;
    completion_minutes = formatcompletionTime->tm_min;
    completion_seconds = formatcompletionTime->tm_sec;

    fprintf(simulation_log, "Number of tasks put into the Ready-Queue: %d\nTerminate at time: %02d:%02d:%02d\n\n", num_tasks, completion_hours, completion_minutes, completion_seconds);

}

/* Write cpu thread completion information to the simulation_log.txt */
void writeCpuThreadComplete(FILE* simulation_log, char* CPU_num, int num_tasks_served_by_this_cpu)
{
    fprintf(simulation_log, "%s terminates after serving %d tasks\n", CPU_num, num_tasks_served_by_this_cpu);
}

/* Write final summary (in main) information to the simulation_log.txt */
void writeFinalSummary(FILE* simulation_log, int num_tasks, double average_waiting_time, double average_turnaround_time)
{
    fprintf(simulation_log, "Number of tasks: %d\nAverage waiting time: %0.2f seconds\nAverage turn around time: %0.2f seconds\n", num_tasks, average_waiting_time, average_turnaround_time);
}
