COMPILER = clang++
INCLUDE = -I include/ -std=c++11
COMPILE = $(COMPILER) -c $(INCLUDE)
LIBS = lib
LINK = $(COMPILER) $(LIBS)/*.o -o
SRC = src
MAIN = main
SUDOKU = sudoku
ROW = Row
COLUMN = Column
MATRIX_3X3 = Matrix_3x3
MATRIX_9X9 = Matrix_9x9
OBJ = $(LIBS)/$(MAIN).o $(LIBS)/$(SUDOKU).o $(LIBS)/$(ROW).o $(LIBS)/$(COLUMN).o $(LIBS)/$(MATRIX_3X3).o $(LIBS)/$(MATRIX_9X9).o
TGT = $(SUDOKU).exe

.PHONY: all run clean

all:	$(OBJ)
	mkdir -p $(LIBS)
	-mv $(?F) $(LIBS)/
	$(LINK) $(TGT) -lncurses

$(LIBS)/$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $<

$(LIBS)/$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
			$(COMPILE) $<

$(LIBS)/$(ROW).o:	$(SRC)/$(ROW).cpp
			$(COMPILE) $<

$(LIBS)/$(COLUMN).o:	$(SRC)/$(COLUMN).cpp
			$(COMPILE) $<

$(LIBS)/$(MATRIX_3X3).o:	$(SRC)/$(MATRIX_3X3).cpp
				$(COMPILE) $<

$(LIBS)/$(MATRIX_9X9).o:	$(SRC)/$(MATRIX_9X9).cpp
				$(COMPILE) $<

run:	$(TGT)
	./$<

clean:	$(TGT)
	rm $< $(LIBS)/*.o
	rmdir $(LIBS)
