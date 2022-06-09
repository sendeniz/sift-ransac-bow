#!/bin/bash
for i in /home/sen/Downloads/mir_a2/bowhomework/bow/bowclassification/data/labradorquery/*.jpg; do
   echo "Using query img $i"
   /home/sen/Downloads/mir_a2/bowhomework/bow/bowclassification/Debug/bowclassification $i
done
