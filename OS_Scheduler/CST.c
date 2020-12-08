#include "CST.h"
#include <math.h>

void cst_swap(Process *ps, Process *cur){ // 노드 교환
    Process tmp;
    tmp.pid = ps->pid;
    tmp.done = ps->done;
    tmp.t_arrival = ps->t_arrival;
    tmp.t_remain = ps->t_remain;
    tmp.t_service = ps->t_service;
    ps->pid = cur->pid;
    ps->done = cur->done;
    ps->t_arrival = cur->t_arrival;
    ps->t_remain = cur->t_remain;
    ps->t_service = cur->t_service;
    cur->pid = tmp.pid;
    cur->done = tmp.done;
    cur->t_arrival = tmp.t_arrival;
    cur->t_remain = tmp.t_remain;
    cur->t_service = tmp.t_service;
}

double getPriority(Process *ps){ // 우선순위 결정 함수 : (1/가중치)*(e/2)^도착시간 + (1/(10-가중치))*서비스시간 
    int a = 10-ps->weight;
    return (0.1*ps->weight)*pow(2.17128/2,ps->t_arrival) + (0.1*a)*pow(2.17128/2,ps->t_service);
    //return pow(2.17128/2,ps->t_arrival) + ps->t_service;
}

void cst_resort(Scheduler *sc){ // 다음 프로세스를 스케줄러에 등록
    double min = getPriority(sc->ps);
    Process *minp = sc->ps;
    Process *cur = sc->ps;
    while(cur){ 
        if(cur->t_arrival <= sc->time){
            if(minp->done == 1)
                if(cur->done != 1){
                    minp = cur;
                    min = getPriority(cur);
                }
            if(min > getPriority(cur) && cur->done != 1){
                minp = cur;
            }
        }
        cur = cur->next;
    }
    cst_swap(sc->ps, minp);
}

int CST_allDone(Scheduler *sc){ // 모두 처리 되었는지 파악
    Process *cur = sc->ps;
    while(cur){
        if(cur->done == 0)
            return 0;
        cur = cur->next;
    }
    return 1;
}

void CST_Schedule(FILE *f, Scheduler *sc){ // 스케줄링
    Process *ps;
    cst_resort(sc);
    ps = sc->ps;
    if(ps->t_remain > 0){
        ps->t_remain--;
        if(ps->t_remain == 0){
            ps->done = 1;
            ps->t_endtime = sc->time+1;
            //pid, 도착시간, 서비스시간, 종료시간, 반환시간, 정규화된 반환시간
            printf("(PID:%d) arrival: %d, service: %d, end: %d, return: %d, return_norm: %.2f\n",ps->pid, ps->t_arrival, ps->t_service, ps->t_endtime, ps->t_endtime-ps->t_arrival, (double)(ps->t_endtime-ps->t_arrival)/ps->t_service);
            
            fprintf(f, "(PID:%d) arrival: %d, service: %d, end: %d, return: %d, return_norm: %.2f\n",ps->pid, ps->t_arrival, ps->t_service, ps->t_endtime, ps->t_endtime-ps->t_arrival, (double)(ps->t_endtime-ps->t_arrival)/ps->t_service);
            sc->return_sum += ps->t_endtime-ps->t_arrival;
        }
    }
    sc->time++;
}
