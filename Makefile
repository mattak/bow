#!/bin/make
CC    = cv++
INC   = -I./include
OPS   = -g

IDIR=./include
SDIR=src
ODIR=obj

_DEP = $(shell ls include)
DEP  = $(patsubst %,$(IDIR)/%,$(_DEP))
_SRC = $(shell ls src)
SRC  = $(patsubst %,$(SDIR)/%,$(_SRC))
_OBJ = $(subst .cpp,.o,$(_SRC))
OBJ  = obj/feature.o obj/book.o obj/util.o #$(patsubst %,$(ODIR)/%,$(_OBJ))
TARGET = desc cbook cword test plinear dump2header
INSTALL_TARGET = desc cbook cword descall plinear

all: $(TARGET)

localbin: $(INSTALL_TARGET)
	for f in $^; do cp $$f $(HOME)/bin/; done

rmlocalbin:
	for f in $(INSTALL_TARGET); do rm $(HOME)/bin/$$f; done

desc: src/desc.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
cbook: src/cbook.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
cword: src/cword.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
test: src/test.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
plinear: src/plinear.cpp 
	$(CC) -o $@ $^ $(INC) $(OPS)
dump2header: src/dump2header.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

obj/util.o: src/util.cpp include/util.h
	$(CC) -c -o $@ $< $(INC) $(OPS)

obj/feature.o: src/feature.cpp include/feature.h include/util.h
	$(CC) -c -o $@ $< $(INC) $(OPS)

obj/book.o: src/book.cpp include/book.h include/feature.h include/util.h
	$(CC) -c -o $@ $< $(INC) $(OPS)

echo:
	@echo "src:" $(SRC)
	@echo "obj:" $(OBJ)
	@echo "inc:" $(DEP)
	@echo "ooo:" %.o
	@echo "c++:" $(CC)

.PHONY: clean

clean:
	@rm -Rf $(ODIR)
	@rm $(TARGET)

