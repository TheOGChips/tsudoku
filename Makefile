COMPILER = g++
INCLUDE_FLAGS = -I $(INCLUDE)/ -std=c++17
COMPILE = $(COMPILER) -c $(INCLUDE_FLAGS)
LIB = lib
INCLUDE = include
LINK = $(COMPILER) $(LIB)/*.o -o
SRC = src
MAIN = main
MENU = Menu
MAIN_MENU = MainMenu
INGAME_MENU = InGameMenu
SUDOKU = sudoku
CONTAINER = Container
ROW = Row
COLUMN = Column
BOX = Box
MATRIX_9X9 = Matrix_9x9
OBJ = $(LIB)/$(MAIN).o $(LIB)/$(MAIN_MENU).o $(LIB)/$(INGAME_MENU).o $(LIB)/$(SUDOKU).o $(LIB)/$(ROW).o $(LIB)/$(COLUMN).o $(LIB)/$(BOX).o $(LIB)/$(MATRIX_9X9).o
TGT = $(SUDOKU).exe

.PHONY: all run clean

all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT) -lncurses

$(LIB)/$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $<

#$(LIB)/$(MENU).o:	$(SRC)/$(MENU).cpp $(INCLUDE)/$(MENU).hpp
#			$(COMPILE) $<

#TODO: Clean this up some
$(LIB)/$(MAIN_MENU).o:	$(SRC)/$(MAIN_MENU).cpp $(INCLUDE)/$(MAIN_MENU).hpp $(INCLUDE)/$(MENU).hpp
			$(COMPILE) $<

$(LIB)/$(INGAME_MENU).o:	$(SRC)/$(INGAME_MENU).cpp $(INCLUDE)/$(INGAME_MENU).hpp $(INCLUDE)/$(MENU).hpp
				$(COMPILE) $<

$(LIB)/$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
			$(COMPILE) $<

$(LIB)/$(ROW).o:	$(SRC)/$(ROW).cpp $(INCLUDE)/$(ROW).hpp $(INCLUDE)/$(CONTAINER).hpp
			$(COMPILE) $<

$(LIB)/$(COLUMN).o:	$(SRC)/$(COLUMN).cpp $(INCLUDE)/$(COLUMN).hpp $(INCLUDE)/$(CONTAINER).hpp
			$(COMPILE) $<

$(LIB)/$(BOX).o:	$(SRC)/$(BOX).cpp $(INCLUDE)/$(BOX).hpp $(INCLUDE)/$(CONTAINER).hpp
			$(COMPILE) $<

$(LIB)/$(MATRIX_9X9).o:	$(SRC)/$(MATRIX_9X9).cpp
				$(COMPILE) $<

run:	$(TGT)
	./$<
	@#rmdir ~/.tsudoku

clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)
