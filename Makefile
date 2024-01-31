SOURCES = mem-bloat.c

EXE = mem-bloat

CFLAGS = -Wall

LDFLAGS =

LIBS =

LD = gcc

OBJECTS = $(SOURCES:%.c=%.o)

default: all

all: $(EXE)

$(EXE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o  $(EXE) $(LIBS)

%.o: %.c
	gcc -o $@ -c $<

clean:
	-rm -f $(EXE)      # Remove the executable file
	-rm -f $(OBJECTS)  # Remove the object files

mem-bloat.o: mem-bloat.c
