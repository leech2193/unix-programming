#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define Fi 0
#define Re 1
#define NR -1
char doo[4];//마지막 글자
char fir[4];//첫번째 글자

struct memory {
    char msg[100];
    int status, pid1, pid2;
};

struct memory* shmptr;

void pt(int signum)//1번선수에게서 온 단어 출력
{
    int slen = strlen(shmptr->msg);//단어의 길이
    for(int i=4; i>1; i--) {
        doo[4-i] = (shmptr->msg)[slen-i];//마지막 글자 추출
    }
    if (signum == SIGUSR1) {//signum이 1번 선수에게서 온 것이라면 출력
        printf("1번선수의 단어: ");
        printf("%s", shmptr->msg);
        printf("시작글자: ");
        printf("%s", doo);
        printf("\n");
    }
}

int main()
{
    int pid = getpid();//2번선수의 pid
    int shmid;

    FILE *fp;
    char test[50];
    fp = fopen("words.txt", "wt");
    fclose(fp);

    int key = 321;  //공유메모리의 키 값

    shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
    //공유메모리 생성

    shmptr = (struct memory*)shmat(shmid, NULL, 0);
    // 공유메모리에 연결

    shmptr->pid1 = pid;//pid를 공유메모리에 저장
    shmptr->status = NR;

    signal(SIGUSR1, pt);

    while (1) {
        while (shmptr->status != Re)
            continue;
        sleep(1);
        
        printf("2번선수: ");
        fgets(shmptr->msg, 100, stdin);//단어 입력
        if(strlen(shmptr->msg) > 13 || strlen(shmptr->msg) < 5) {
            printf("단어가 너무 길거나 짧습니다: 2번선수 탈락\n");
            remove("words.txt");
            exit(1);
        }
        
        for(int i=0; i<3; i++) {
            fir[i] = (shmptr->msg)[i];//입력한 단어의 첫번째 글자 추출
        }
        if(strcmp(fir, doo) != 0) {//만약 첫번째 글자와 상대방의 마지막 글자가 같지 않다면
            printf("끝말잇기 실패: 2번선수 탈락\n");//탈락
            remove("words.txt");
            exit(1);
        }
        
        fp = fopen("words.txt", "rt");//txt 파일 읽기모드로 오픈
        while(fgets(test, sizeof(test), fp) != NULL) {//word 파일의 단어 하나씩 읽음
            if(strcmp(shmptr->msg, test) == 0) {//만약 내가 적은 단어와 같다면
                printf("단어 중복: 2번선수 탈락\n");//탈락
                remove("words.txt");
                exit(1);
            }
        }
        fclose(fp);//파일 닫음
        
        fp = fopen("words.txt", "a+");//파일 append 모드로 오픈
        if(fp==NULL) {//파일이 없다면 에러
            printf("실패\n");
            return 1;
        }
        fprintf(fp, "%s", shmptr->msg);//내가 적은 단어를 파일에 덧붙여 쓴다
        fclose(fp);//파일 닫음
        
        shmptr->status = Fi;
        kill(shmptr->pid2, SIGUSR2);//다른 유저에게 메시지 전송
    }
    
    shmdt((void*)shmptr);//공유메모리 연결 해제
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

