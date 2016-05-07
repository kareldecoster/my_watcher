IDIR = ./include
CFLAGS = -Wall -Werror -g -I$(IDIR)
CC = gcc

ODIR = ./src/obj

_DEPS =  #.h files
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = watcher.o#.o files
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	

all: $(OBJ)
	make mercator
	
mercator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lfftw3 -lm
		
clean: 
	$ rm -f $(ODIR)/*.o core $(INCDIR)/*~
	$ rm -f ./watcher
	$ clear
	
	
.PHONY: clean
