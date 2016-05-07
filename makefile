IDIR = ./include
CFLAGS = -Wall -Werror -g -I$(IDIR)
CC = gcc

ODIR = ./src/obj

_DEPS =  config.h#.h files
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = watcher.o#.o files
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TEST_OBJ = test.o#.o files
TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_OBJ))
$(ODIR)/%.o: ./src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	

all: $(OBJ) $(OBJ_T)
	make watcher
	make test
	
watcher: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lfftw3 -lm -lwiringPi
	
test: $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lwiringPi

		
clean: 
	$ rm -f $(ODIR)/*.o core $(INCDIR)/*~
	$ rm -f ./watcher
	$ clear
	
	
.PHONY: clean
