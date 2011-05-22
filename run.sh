#!/bin/bash

# This script calls the unet program with varying combinations of arguments,
# and redirects the output streams (stdout and stderr) to files.

# WARNING: Running this script will produce lots of files!

i=1

for as_tres in 0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45
do
  for as_step in 0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45
  do
    for ln_tres in 0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45
    do
      echo "Performing simulation $i of 729..."
      ./unet 1000 5000 $as_tres $as_step $ln_tres \
       > 1000.5000.$as_tres.$as_step.$ln_tres.stdout \
      2> 1000.5000.$as_tres.$as_step.$ln_tres.stderr
      i=$((i+1))
    done
  done
done
