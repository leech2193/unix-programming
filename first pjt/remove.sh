#!/bin/bash

destDir=./cemetery #cemetery 디렉토리를 목표로 지정
dday=("일" "월" "화" "수" "목" "금" "토") #요일

for i in $*
do	
	if(mv $i $destDir)#입력된 파일 또는 디렉토리를 cemetery파일로 이동(삭제)
	then
		date '+%Y년 %m월 %d일 %A %H시 %M분 %S초, '$i'은(는) 공동묘지에 묻혔습니다...'
	fi
done

exit 0
