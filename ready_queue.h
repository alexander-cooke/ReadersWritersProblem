/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name: Alex Cooke (18406489)
 * File name: ready_queue.h
 * Purpose: This header file declares an appropriate struct for representing
 * a single task from task_file
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <time.h>

#ifndef READY_QUEUE_H
#define READY_QUEUE_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Struct name: Task
 * Purpose: Each Task contains the task's task#, cpu_burst and arrival_time
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef struct
{
    int task_ID;
    int cpu_burst;
    time_t arrival_time;
} Task;

#endif
