#!/bin/bash

echo Enter experiment reference
read experimentReference

echo Enter file to analyse
read filename

echo Enter resolution in Hz
read resolution

#mkdir "$experimentReference"

echo Analysing frequency content
python3 frequency\ analyser.py "$filename" "$resolution" "$experimentReference"

echo Finding frequency peaks
python3 frequency_peaks_finder.py "$experimentReference"

echo Plotting peaks
python3 Peak_following_plotter.py "$experimentReference"




#everything past here is examples
