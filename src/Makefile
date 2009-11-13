CC          = gcc
AR          = ar
CFLAGS      = -Wall -g
INCLUDES    = -I../include
LIBS        = -L. -lNGR_store -lc
EXECUTABLES = ngr_info ngr_insert ngr_create ngr_dump ngr_agg
SOURCES     = ${EXECUTABLES:=.c}


all: ${EXECUTABLES}

NGR_store: 
	${CC} -c ${CFLAGS} ${INCLUDES} NGR_store.c -o NGR_store.o
	${AR} rcs libNGR_store.a NGR_store.o

${EXECUTABLES}: NGR_store
	${CC} ${CFLAGS} ${INCLUDES} ${LIBS} $@.c -o $@

clean:
	rm -f *.o core *.core *.a ${EXECUTABLES}

