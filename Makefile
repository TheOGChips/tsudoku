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
GRID = Grid
OBJ = $(LIB)/$(MAIN).o $(LIB)/$(MAIN_MENU).o $(LIB)/$(INGAME_MENU).o $(LIB)/$(SUDOKU).o $(LIB)/$(GRID).o $(LIB)/$(CONTAINER).o
TGT = $(SUDOKU).exe

.PHONY: all run clean

all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT) -lncurses

$(LIB)/$(MAIN).o:	$(SRC)/$(MAIN).cpp
			$(COMPILE) $<

#TODO: Clean this up some
$(LIB)/$(MAIN_MENU).o:	$(SRC)/$(MAIN_MENU).cpp $(INCLUDE)/$(MAIN_MENU).hpp $(INCLUDE)/$(MENU).hpp
			$(COMPILE) $<

$(LIB)/$(INGAME_MENU).o:	$(SRC)/$(INGAME_MENU).cpp $(INCLUDE)/$(INGAME_MENU).hpp $(INCLUDE)/$(MENU).hpp
				$(COMPILE) $<

$(LIB)/$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp
			$(COMPILE) $<

$(LIB)/$(GRID).o:	$(SRC)/$(GRID).cpp
			$(COMPILE) $<

$(LIB)/$(CONTAINER).o:	$(SRC)/$(CONTAINER).cpp $(INCLUDE)/$(CONTAINER).hpp
			$(COMPILE) $<

run:	$(TGT)
	./$<
	@#rmdir ~/.tsudoku

clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)
