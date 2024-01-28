#!/bin/bash

FORMAT="|%-30s|%10s|%10s|%10s|%10s|\n"
ITERATIONS1=1000000
ITERATIONS2=10000000


processor_type=$(lscpu | grep "Model name" | cut -d ':' -f 2 | sed 's/^ *//')
architecture=$(uname -i)
os_name=$(uname -o)
echo "Processor Type: $processor_type"
echo "Architecture: $architecture"
echo "OS: $os_name"


DIVISION=$(printf "$FORMAT" | tr " " "-" | tr "|" "+")
printf "%s\n" "$DIVISION"
printf "$FORMAT" "program" "time [ms]" "iterations" "time [ms]" "iterations"
printf "%s\n" "$DIVISION"

for program in "$@"; do
  program_name=$(printf "%s" $program | sed s/.out$/.cpp/)
  printf "$FORMAT" "$program_name" "$(./${program} $ITERATIONS1)" "$ITERATIONS1" "$(./${program} $ITERATIONS2)" "$ITERATIONS2"
done

printf "%s\n" "$DIVISION"
