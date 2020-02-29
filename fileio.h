
#ifndef FILEIO_H
#define FILEIO_H

/* * * * * * * * * * * * * * function prototypes * * * * * * * * * * */
int getItem(Task* task, FILE* task_file);
void writeArrivalTask(Task* task, FILE* simulation_log);
void writeCPUServiceTask(FILE* simulation_log, Task* task, char* CPU_num, time_t service_time);
void writeCPUCompleteTask(FILE* simulation_log, Task* task, char* CPU_num, time_t completion_time);
void writeTaskThreadComplete(FILE* simulation_log, int num_tasks);
void writeCpuThreadComplete(FILE* simulation_log, char* CPU_num, int num_tasks);
void writeFinalSummary(FILE* simulation_log, int num_tasks, double average_waiting_time, double average_turnaround_time);
#endif
