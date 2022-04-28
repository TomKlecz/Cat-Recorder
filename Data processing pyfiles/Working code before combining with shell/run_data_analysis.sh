#!/bin/bash

echo Enter file to analyse
read filename

echo Enter experiment reference
read experimentReference


python3 frequency\ analyser.py "$filename" "$experimentReference"




#everything past here is examples
