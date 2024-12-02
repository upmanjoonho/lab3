#!/bin/bash
echo "종료하려면 'exit'를 입력하세요."

while true; do
    read input
    if [ "$input" == "exit" ]; then
        echo "프로그램을 종료합니다."
        break
    else
        echo "입력하신 내용: $input. 종료하려면 'exit'를 입력하세요."
    fi
done
