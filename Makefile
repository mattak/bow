#!/bin/make
CC    = cv++
INC   = -I./include

IDIR=./include
SDIR=src
ODIR=obj

_DEP = $(shell ls include)
DEP  = $(patsubst %,$(IDIR)/%,$(_DEP))
_SRC = $(shell ls src)
SRC  = $(patsubst %,$(SDIR)/%,$(_SRC))
_OBJ = $(subst .cpp,.o,$(_SRC))
OBJ  = $(patsubst %,$(ODIR)/%,$(_OBJ))
TARGET = desc

#$(ODIR)/%.o: %.cpp $(DEP) 
#	$(CC) -c -o $@ $< $(INC)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(INC)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

obj/main.o: src/main.cpp
	$(CC) -c -o $@ $< $(INC)

obj/bow.o: src/bow.cpp $(DEP)
	$(CC) -c -o $@ $< $(INC)


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

