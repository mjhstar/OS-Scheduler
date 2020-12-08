#include <stdio.h>
#include <stdlib.h>

#ifndef _SCHEDULER_
#define _SCHEDULER_
// 프로세스
typedef struct _process{
    int pid;
    int t_arrival;
    int t_service;
    int t_remain;
    int t_endtime;
    int done;
    int weight; //가중치
    struct _process *next;
}Process;
// 스케줄러
typedef struct _scheduler{
    int time;
    double return_sum;
    int numOfPs;
    Process *ps;
}Scheduler;

Scheduler *newScheduler(void);
Process *newProcess(Scheduler *sc, int _pid, int arr, int ser);
Process *newProcess1(Scheduler *sc, int _pid, int arr, int ser, int w);//우선순위 전용 함수
void addProcessFromFile(const char *filename, Scheduler *sc);
void addProcessFromFile1(const char *filename, Scheduler *sc, int w);//우선순위 전용 함수
void printScheduler(Scheduler *sc);

#endif /* _A_HEADER_H__ */
