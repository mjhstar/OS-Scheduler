#include "scheduler.h"
#include <string.h>
#include "SRT.h"
#include "SPN.h"
#include "CST.h"

/*
 
 프로그램 출력:
 하나의 프로세스 종료될 때마다 다음 6가지 출력
 pid, 도착시간, 서비스시간, 종료시간, 반환시간, 정규화된 반환시간
 중간과정은 별도의 파일에 저장(output.txt)
 
 종료시간 : 종료된 시간
 반환시간 : 종료까지 걸린 시간
 정규화된 반환시간 : 반환시간/서비스시간
 
 */

Scheduler *newScheduler(){ // 스케줄러 생성
    Scheduler *sc = (Scheduler *)malloc(sizeof(Scheduler));
    sc->ps = NULL;
    sc->time = 0;
    sc->return_sum = 0;
    sc->numOfPs = 0;
    return sc;
}

Process *newProcess(Scheduler *sc, int _pid, int arr, int ser){ // 프로세스 생성
    Process *process = (Process *) malloc(sizeof(Process));
    Process *cur = NULL;
    process->pid = _pid;
    process->t_arrival = arr;
    process->t_service = ser;
    process->t_remain = ser;
    process->t_endtime = 0;
    process->done = 0;
    process->weight = 0;
    process->next = NULL;
    if(sc->ps == NULL){
        sc->ps = process;
    }else{
        cur = sc->ps;
        while(cur->next){
            cur = cur->next;
        }
        cur->next = process;
    }
    return process;
}
Process *newProcess1(Scheduler *sc, int _pid, int arr, int ser , int w){ // 프로세스 생성 (우선순위) 전용
    Process *process = (Process *) malloc(sizeof(Process));
    Process *cur = NULL;
    process->pid = _pid;
    process->t_arrival = arr;
    process->t_service = ser;
    process->t_remain = ser;
    process->t_endtime = 0;
    process->done = 0;
    process->weight = w;
    process->next = NULL;
    if(sc->ps == NULL){
        sc->ps = process;
    }else{
        cur = sc->ps;
        while(cur->next){
            cur = cur->next;
        }
        cur->next = process;
    }
    return process;
}

void addProcessFromFile(const char *filename, Scheduler *sc){ // 파일로부터 프로세스읽어서 스케줄러에 등록
    FILE *f = fopen(filename, "r");
    int pid, arr, ser;
    char buf[500];
    if(f == 0){
        printf("File Do Not Exists\n");
        exit(1);
    }
    while(fgets(buf, sizeof(buf), f) != NULL){
        sscanf(buf, "%d,%d,%d", &pid, &arr, &ser);
        newProcess(sc, pid, arr, ser);
        sc->numOfPs++;
    }
    fclose(f);
}
void addProcessFromFile1(const char *filename, Scheduler *sc, int w){ // 파일로부터 프로세스읽어서 스케줄러에 등록 (우선순위)
    FILE *f = fopen(filename, "r");
    int pid, arr, ser;
    char buf[500];
    if(f == 0){
        printf("File Do Not Exists\n");
        exit(1);
    }
    while(fgets(buf, sizeof(buf), f) != NULL){
        sscanf(buf, "%d,%d,%d", &pid, &arr, &ser);
        newProcess1(sc, pid, arr, ser, w);
        sc->numOfPs++;
    }
    fclose(f);
}

void printScheduler(Scheduler *sc){ // 스케줄러 정보 출력 (안씀)
    Process *ps = sc->ps;
    printf("Time : %d\n", sc->time);
    while(ps){
        if(ps->t_arrival <= sc->time && ps->done == 0)
        printf("(pid:%d) arrival = %d service = %d, remain = %d\n", ps->pid, ps->t_arrival, ps->t_service, ps->t_remain);
        ps = ps->next;
    }
}

int main(int argc, const char * argv[]) { // 메인
    Scheduler *scSRT, *scSPN, *scCST;
    FILE *f_spn, *f_srt, *f_cst; // 각각의 스케줄링 결과 저장 하기 위함
    char filename[500]; // 파일이름
    if(argc == 2){
        strcpy(filename, argv[1]); // 명령인수로 파일이름 받음
    }
    else{
        printf("<usage> : %s <filename>\n", argv[0]);
        exit(1);
    }
    while(1){
        printf("1.SPN 2.SRT 3.CST(Priority) 4.Quit : ");
        int select = 0;
        scanf("%d",&select);
        switch(select){
            case 1: // SPN 스케줄링
                scSPN = newScheduler(); // 스케줄러
                f_spn = fopen("spn_output.txt", "w");
                addProcessFromFile(filename, scSPN); // 프로세스 추가

                printf("SPN\n--------------\n"); // 출력
                for(scSPN->time = 0; SPN_allDone(scSPN)==0;){
                    SPN_Schedule(f_spn, scSPN);
                }
                printf("return mean : %.2f\n", scSPN->return_sum/scSPN->numOfPs);
                fprintf(f_spn, "return mean : %.2f\n", scSPN->return_sum/scSPN->numOfPs);
                printf("--------------\n");

                fclose(f_spn);
                break;
            case 2: // SRT 스케줄링
                scSRT = newScheduler(); // 스케줄러
                f_srt = fopen("srt_output.txt", "w");
                addProcessFromFile(filename, scSRT); // 프로세스 추가

                printf("SRT\n--------------\n"); // 출력
                for(scSRT->time = 0; SRT_allDone(scSRT)==0;){
                    SRT_Schedule(f_srt, scSRT);
                }
                printf("return mean : %.2f\n", scSRT->return_sum/scSRT->numOfPs);
                fprintf(f_srt, "return mean : %.2f\n", scSRT->return_sum/scSRT->numOfPs);
                printf("--------------\n");

                fclose(f_srt);
                break;
            case 3: //우선순위 스케줄링
                printf("도착시간에 부여할 가중치 (1~9) 입력 :  ");
                int w = 0;
                scanf("%d",&w);
                scCST = newScheduler(); // 스케줄러
                f_cst = fopen("cst_output.txt", "a+");
                addProcessFromFile1(filename, scCST, w); // 프로세스 추가

                printf("CST\n--------------\n"); // 출력
                fprintf(f_cst,"가중치 : %d \n",w);
                for(scCST->time = 0; CST_allDone(scCST)==0;){
                    CST_Schedule(f_cst, scCST);
                }
                printf("return mean : %.2f\n", scCST->return_sum/scCST->numOfPs);
                fprintf(f_cst, "return mean : %.2f\n", scCST->return_sum/scCST->numOfPs);
                fprintf(f_cst,"\n");
                printf("--------------\n");

                fclose(f_cst);
                break;
            case 4: // 종료
                printf("종료합니다. \n");
                return 0;
            default: // 잘못 입력
                printf("잘못 입력 했습니다. \n");
                printf("종료합니다. \n");
                return 0;
        }
    }

    return 0;
}
