TARGET=cpuactivity

#compiler
CC=gcc
#debug
DEBUG=-g
#optimisation
OPT=-O0
#warnings
WARN=-Wall
#c files
C_SOURCE=$(wildcard ./source/*.c)

#headers
H_SOURCE=$(wildcard ./source/*.h)

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

#linker

LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic

OBJS=	$(subst .c,.o,$(subst source,objects,$(C_SOURCE)))

all:objFolder $(TARGET)

$(TARGET): $(OBJS)
	@ echo 'Building binary using GCC linker: $@'
	$(LD) -o $@ $(OBJS) $(LDFLAGS)
	@ echo 'Finishid Building binary: $@'
	@ echo ' '

./objects/%.o: ./source/%.c ./source./%.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '
./objects/main.o: ./source/main.c $(H_SOURCE)
	@ echo 'Building target using GCC compiler: $<'
	$(CC) -c $(CCFLAGS) $< $(GTKLIB) -o $@
	@ echo ' '

objFolder:
	@ mkdir -p objects

clean:
	@ rm -rf ./objects/*.o $(TARGET) *~
	@ rmdir objects

.PHONY: all clean
