COMPILER = clang++
COMPILE = $(COMPILER) -c
LINK = $(COMPILER) -o
SRC = src
MAIN = main
SUDOKU = sudoku
ROW = Row
COLUMN = Column
MATRIX_3X3 = Matrix_3x3
MATRIX_9X9 = Matrix_9x9
OBJ = $(MAIN).o $(SUDOKU).o $(ROW).o $(COLUMN).o $(MATRIX_3X3).o $(MATRIX_9X9).o
TGT = $(SUDOKU).exe

all:	$(OBJ)
		$(LINK) $(TGT) $(OBJ)

$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $(SRC)/$(MAIN).cpp

$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
				$(COMPILE) $(SRC)/$(SUDOKU).cpp

$(ROW).o:	$(SRC)/$(ROW).cpp
			$(COMPILE) $(SRC)/$(ROW).cpp

$(COLUMN).o:	$(SRC)/$(COLUMN).cpp
				$(COMPILE) $(SRC)/$(COLUMN).cpp

$(MATRIX_3X3).o:	$(SRC)/$(MATRIX_3X3).cpp
					$(COMPILE) $(SRC)/$(MATRIX_3X3).cpp

$(MATRIX_9X9).o:	$(SRC)/$(MATRIX_9X9).cpp
					$(COMPILE) $(SRC)/$(MATRIX_9X9).cpp

.PHONY: run clean
run: $(TGT)
	 ./$(TGT)

clean:	$(TGT)
		rm $(TGT) $(OBJ)
