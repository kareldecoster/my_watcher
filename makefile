IDIR = ./include
CFLAGS = -Wall -Werror -g -I$(IDIR)
CC = gcc

ODIR = ./src/obj

_DEPS =  #.h files
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = watcher.o#.o files
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TEST = test.o
TEST = $(patsusbst %,$(ODIR)/%,$(_TEST))

$(ODIR)/%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	

all: $(OBJ)
	make watcher
	make test
	
watcher: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lfftw3 -lm
	
test: $(TEST)
    $(CC) -o $@ $^ $(CFLAGS)
		
clean: 
	$ rm -f $(ODIR)/*.o core $(INCDIR)/*~
	$ rm -f ./watcher
	$ clear
	
	
.PHONY: clean
