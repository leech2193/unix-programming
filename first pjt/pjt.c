#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

int emptytc(const char *path, char *argv[]) {
    DIR * dirloc = NULL;
    struct dirent *file = NULL;
    struct stat buf;
    char fildir[128]; // 디렉토리명 저장. DIR에서 반환받음
    
    if((dirloc = opendir(path)) == NULL) {//디렉토리가 아니면 삭제
        return unlink(path);
    }
    
    while((file = readdir(dirloc)) != NULL) {//디렉토리 안의 파일과 디렉토리를 읽는다
        //읽혀진 파일명 중에 현재 디렉토리인 . 이나 부모디렉토리인 ..도 있어서, 있으면 스킵한다
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        sprintf(fildir, "%s/%s", path, file->d_name);
        
        if(lstat(fildir, &buf) == -1) {//파일의 정보를 얻는다
            continue;
        }
        if(S_ISDIR(buf.st_mode)) {//디렉토리라면
            if(emptytc(fildir, argv) == -1) {//재귀함수로 다시 실행
                return -1;
            }
        }
        else if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {//일반파일이거나 심볼릭파일이면
            if(unlink(fildir) == -1) {//삭제
                return -1;
            }
        }
    }
    closedir(dirloc);
    if(path == argv[1]) { //cemetery 디렉토리(휴지통)라면 삭제하지 않는다
        return 0;
    }
    else { //그 외의 디렉토리는 삭제
        return rmdir(path);
    }
}

int main(int argc, char *argv[]) {
    struct tm *tm;
    time_t t;
    char *dday[] = {"일", "월", "화", "수", "목", "금", "토"};
    time(&t);
    tm = localtime(&t);
        
    int i, c, w;
    
    mkdir("cemetery", 0755);
    
    while (--argc > 0 && (*++argv)[0] == '-') { //옵션을 입력받은 경우
        while((c = *++argv[0])) {
            switch(c) {
                case 'd': //옵션 d(삭제)를 입렫받은 경우
                    if(((w = execv("./remove.sh", argv)) != -1)) {
                        perror("Delete");
                        exit(1);
                    }
                    break;

                case 't'://휴지통 비우기
                    emptytc(argv[1], argv);
                    printf("%d년 %d월 %d일 %s요일 %d시 %d분 %d초, 파일들은 돌아올 수 없는 강을 건넜습니다...\n", (tm->tm_year)+1900, (tm->tm_mon)+1, tm->tm_mday, dday[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
                    break;
                
                case 'r'://파일/디렉토리 복구
                    if(((w = execv("./recover.sh", argv)) != -1)) {
                        perror("Recover");
                        exit(1);
                    }
                    break;
                                
                default: //부적절한 옵션을 입력했을 경우
                    printf("부적절한 옵션 -- '%c'\n", c);
                    break;
            }
        }
    }
    return 0;
}
