COMPILER = g++
INCLUDE = -I include/ -std=c++17
COMPILE = $(COMPILER) -c $(INCLUDE)
LIB = lib
LINK = $(COMPILER) $(LIB)/*.o -o
SRC = src
MAIN = main
MENU = Menu
SUDOKU = sudoku
ROW = Row
COLUMN = Column
MATRIX_3X3 = Matrix_3x3
MATRIX_9X9 = Matrix_9x9
OBJ = $(LIB)/$(MAIN).o $(LIB)/$(MENU).o $(LIB)/$(SUDOKU).o $(LIB)/$(ROW).o $(LIB)/$(COLUMN).o $(LIB)/$(MATRIX_3X3).o $(LIB)/$(MATRIX_9X9).o
TGT = $(SUDOKU).exe

.PHONY: all run clean

all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT) -lncurses

$(LIB)/$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $<

$(LIB)/$(MENU).o:	$(SRC)/$(MENU).cpp
			$(COMPILE) $<

$(LIB)/$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
			$(COMPILE) $<

$(LIB)/$(ROW).o:	$(SRC)/$(ROW).cpp
			$(COMPILE) $<

$(LIB)/$(COLUMN).o:	$(SRC)/$(COLUMN).cpp
			$(COMPILE) $<

$(LIB)/$(MATRIX_3X3).o:	$(SRC)/$(MATRIX_3X3).cpp
				$(COMPILE) $<

$(LIB)/$(MATRIX_9X9).o:	$(SRC)/$(MATRIX_9X9).cpp
				$(COMPILE) $<

run:	$(TGT)
	./$<
	@#rmdir ~/.tsudoku

clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)
