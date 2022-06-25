COMPILER = clang++
INCLUDE = -I include/
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
OBJ = $(MAIN).o $(SUDOKU).o $(ROW).o $(COLUMN).o $(MATRIX_3X3).o $(MATRIX_9X9).o
TGT = $(SUDOKU).exe

.PHONY: all run clean

all:	$(OBJ)
		mkdir $(LIBS)
		mv $^ $(LIBS)/
		$(LINK) $(TGT)

$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $<

$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
				$(COMPILE) $<

$(ROW).o:	$(SRC)/$(ROW).cpp
			$(COMPILE) $<

$(COLUMN).o:	$(SRC)/$(COLUMN).cpp
				$(COMPILE) $<

$(MATRIX_3X3).o:	$(SRC)/$(MATRIX_3X3).cpp
					$(COMPILE) $<

$(MATRIX_9X9).o:	$(SRC)/$(MATRIX_9X9).cpp
					$(COMPILE) $<

run: $(TGT)
	 ./$<

clean:	$(TGT)
		rm $< $(LIBS)/*.o
		rmdir $(LIBS)
