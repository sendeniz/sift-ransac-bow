#!/bin/bash
counter = 1
for i in /home/sen/Downloads/mir_a2/bowhomework/bow/bowimageretrieval/data/queryimages/*.jpg; do
   echo "Using query img $i"
   /home/sen/Downloads/mir_a2/bowhomework/bow/bowimageretrieval/Debug/bowimageretrieval $i ranklist$counter.html
   counter=$((counter+1))
done
