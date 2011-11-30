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
TARGET = desc main bk putsvm

#$(ODIR)/%.o: %.cpp $(DEP) 
#	$(CC) -c -o $@ $< $(INC)

all: $(TARGET)

#$(TARGET): $(OBJ)
#	$(CC) -o $@ $^ $(INC)
desc: src/desc.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
main: src/main.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
bk: src/bk.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)
putsvm: src/putsvm.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(OPS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

#obj/main.o: src/main.cpp
#	$(CC) -c -o $@ $< $(INC) $(OPS)

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

