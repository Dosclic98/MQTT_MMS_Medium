#!/bin/bash

if [ $# -ne 6 ]; then
    echo "Usage: $0 -p <path> -f <file_name_prefix> -n <numruns>"
    exit 0;
fi
while getopts p:f:n: flag
do
    case "${flag}" in
        p) path=${OPTARG};;
        f) fileNamePrefix=${OPTARG};;
	n) numRuns=${OPTARG};;
    esac
done

for (( i = 0; i < numRuns; i++ )) 
do
    fullPath="$path"/run"$i"
    
    runFileName=$fileNamePrefix"-#"$i".csv" 
    if [ $i -eq 0 ]; then
	sed "1q;d" "${fullPath}/${runFileName}" > "$path"/"$fileNamePrefix".csv
    fi
    sed "2q;d" "${fullPath}/${runFileName}" >> "$path"/"$fileNamePrefix".csv
done

