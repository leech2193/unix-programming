#!/bin/bash

destDir=./cemetery/$1 #다시 복구하고자 하는 파일
mvDir=./cemetery/.. #cemetery디렉토리의 부모디렉토리
dday=("일" "월" "화" "수" "목" "금" "토")

for i in $*
do	
	if(mv $destDir $mvDir) #cemetery로 삭제된 파일/디렉토리를 삭제하기 전 위치로 복구
	then
		date '+%Y년 %m월 %d일 %A %H시 %M분 %S초, '$i'은(는) 다시 살아 돌아왔습니다...!!'
	fi
done

exit 0
