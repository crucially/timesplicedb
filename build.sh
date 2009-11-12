#/bin/bash

gcc -c NGR_store.c -o NGR_store.o
ar rcs libNGR_store.a NGR_store.o

cc   ngr.c -L. -lNGR_store -o ngr

