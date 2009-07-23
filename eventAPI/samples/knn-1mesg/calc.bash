cat log/6thread/10/* | grep CLA | grep EXE | sed 's\CLASSIFIER_EXEC_TIME=\\' | awk 'BEGIN{total = 0; time=0}{print total++; time+=$1}END{print total" "time" "(time/total)" "2277.21/(time/total)}'
