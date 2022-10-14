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

struct memory { 
	char msg[100];
	int status, pid1, pid2; 
}; 

char doo[4] = "l";
char fir[4] = "l";
int turn = 1;

struct memory* shmptr;

void pt(int signum) {//2번선수에게서 온 메시지 출력
    int slen = strlen(shmptr->msg);
    for(int i=4; i>1; i--) {
        doo[4-i] = (shmptr->msg)[slen-i];
    }
    
    if (signum == SIGUSR2) {//signum이 2번 선수에게서 온 것이라면 출력
		printf("Received From User1: "); 
        printf("%s", shmptr->msg);
        printf("시작글자: ");
        printf("%s", doo);
        printf("\n");
	}
} 

int main() {
	int pid = getpid();
	int shmid;

	int key = 321; //공유메모리의 키 값

    FILE *fp;
    char test[50];
    int i;
    fp = fopen("words.txt", "wt");
    fclose(fp);
    
	shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
    //공유메모리 생성

	shmptr = (struct memory*)shmat(shmid, NULL, 0);//공유메모리에 연결

	shmptr->pid2 = pid;
	shmptr->status = NR;

	signal(SIGUSR2, pt); //SIGUSR2을 이용해서 시그널 실행

	while (1) { 
		sleep(1);
        
		printf("1번선수: "); 
		fgets(shmptr->msg, 100, stdin);//1번선수로부터 단어 입력받음
        if(strlen(shmptr->msg) > 13 || strlen(shmptr->msg) < 4) {
            //단어가 2,3자가 아니라면(2자보다 짧거나 3자보다 길다면
            printf("단어가 너무 길거나 짧습니다: 1번선수 탈락\n");//탈락
            remove("words.txt");
            exit(1);
        }
		shmptr->status = Re;

        for(int i=0; i<3; i++) {//첫번째 글자 추출
            fir[i] = (shmptr->msg)[i];
        }
        if(turn != 1) {
            if(strcmp(fir, doo) != 0) {//나의 첫번째 글자가 상대편의 마지막 글자와 다르면
                printf("끝말잇기 실패: 1번선수 탈락\n");//끝말잇기에 실패, 탈락
                remove("words.txt");
                exit(1);
            }
        }
        
        fp = fopen("words.txt", "rt");//파일을 읽기모드로 연다
        while(fgets(test, sizeof(test), fp) != NULL) {//word 파일의 단어 하나씩 읽음
            if(strcmp(shmptr->msg, test) == 0) {//만약 내가 적은 단어와 같다면
                printf("단어 중복: 1번선수 탈락\n");//중복이므로 탈락
                remove("words.txt");
                exit(1);
            }
        }
        fclose(fp);
        
        fp = fopen("words.txt", "a+");
        if(fp==NULL) {
            printf("실패\n");
            return 1;
        }
        fprintf(fp, "%s", shmptr->msg);//내가 적은 단어를 파일에 덧붙여 쓴다
        fclose(fp);//파일 닫음
		
		kill(shmptr->pid1, SIGUSR1);//다른 유저에게 메시지 전송

		while (shmptr->status == Re)
			continue;
        turn++;
	} 

	shmdt((void*)shmptr);//공유메모리 연결 해제
	return 0; 
} 

