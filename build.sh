#/bin/bash

gcc -c NGR_store.c -o NGR_store.o
ar rcs libNGR_store.a NGR_store.o

#cc   ngr.c -L. -lNGR_store -o ngr
cc   ngr_info.c -L. -lNGR_store -o ngrinfo

cc   ngr_create.c -L. -lNGR_store -o ngrcreate

cc   ngr_insert.c -L. -lNGR_store -o ngrinsert



