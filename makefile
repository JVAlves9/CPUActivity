TARGET=cpuactivity

#compiler
CC=gcc
#debug
DEBUG=-g
#optimisation
OPT=-O0
#warnings
WARN=-Wall

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

#linker

LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic

OBJS=	objects/main.o \
		objects/cpuUsage.o \
		objects/listOfProcess.o \
		objects/processes.o

all: $(OBJS)
	@ echo 'Building binary using GCC linker: $@'
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@ echo 'Finishid Building binary: $@'
	@ echo ' '

objects/listOfProcess.o: src/listOfProcess.c src/listOfProcess.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '
objects/cpuUsage.o: src/cpuUsage.c src/cpuUsage.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '
objects/processes.o: src/processes.c src/processes.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '
objects/main.o: src/main.c
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '

clean:
	@ rm -rf objects/*.o $(TARGET) *~

