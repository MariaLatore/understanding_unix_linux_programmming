#!/bin/sh
words=(
  "h" "e" "l" "l" "o" "w" "o" "r" "l" "d" "s" "h" "e" "l" "o" "v" "e" "s" "m" "e"
)
for c in ${words[@]}; do
   echo $c >> data &
done
wait
