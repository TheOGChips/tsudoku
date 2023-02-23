COMPILER = clang++
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
SAVED_GAME_MENU = SavedGameMenu
DIFFICULTY_MENU = DifficultyMenu
SUDOKU = Sudoku
CONTAINER = Container
GRID = Grid
MISC = misc
OBJ = $(LIB)/$(MISC).o $(LIB)/$(MAIN).o $(LIB)/$(MENU).o $(LIB)/$(MAIN_MENU).o $(LIB)/$(SAVED_GAME_MENU).o $(LIB)/$(DIFFICULTY_MENU).o $(LIB)/$(INGAME_MENU).o $(LIB)/$(SUDOKU).o $(LIB)/$(GRID).o $(LIB)/$(CONTAINER).o
COMMON_HDRS = $(INCLUDE)/colors.hpp $(INCLUDE)/$(MISC).hpp
# TODO: Fix this later
TGT = t$(SUDOKU)

.PHONY: all run_all run run_no_menu run_help run_invalid run_too_many clean

all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT) -lncurses

$(LIB)/$(MISC).o:	$(SRC)/$(MISC).cpp $(COMMON_HDRS)
			$(COMPILE) $<

$(LIB)/$(MAIN).o:	$(SRC)/$(MAIN).cpp $(COMMON_HDRS)
			$(COMPILE) $<

$(LIB)/$(MENU).o:	$(SRC)/$(MENU).cpp $(INCLUDE)/$(MENU).hpp $(COMMON_HDRS)
			$(COMPILE) $<

#TODO: Clean this up some
$(LIB)/$(MAIN_MENU).o:	$(SRC)/$(MAIN_MENU).cpp $(INCLUDE)/$(MAIN_MENU).hpp $(INCLUDE)/$(MENU).hpp $(COMMON_HDRS)
			$(COMPILE) $<

$(LIB)/$(INGAME_MENU).o:	$(SRC)/$(INGAME_MENU).cpp $(INCLUDE)/$(INGAME_MENU).hpp $(INCLUDE)/$(MENU).hpp $(COMMON_HDRS)
				$(COMPILE) $<

$(LIB)/$(SAVED_GAME_MENU).o:	$(SRC)/$(SAVED_GAME_MENU).cpp $(INCLUDE)/$(SAVED_GAME_MENU).hpp $(INCLUDE)/$(MENU).hpp $(COMMON_HDRS)
				$(COMPILE) $(SRC)/$(SAVED_GAME_MENU).cpp

$(LIB)/$(DIFFICULTY_MENU).o:	$(SRC)/$(DIFFICULTY_MENU).cpp $(INCLUDE)/$(DIFFICULTY_MENU).hpp $(INCLUDE)/$(MENU).hpp $(COMMON_HDRS)
				$(COMPILE) $(SRC)/$(DIFFICULTY_MENU).cpp

$(LIB)/$(SUDOKU).o:	$(SRC)/$(SUDOKU).cpp $(COMMON_HDRS)
			$(COMPILE) $<

$(LIB)/$(GRID).o:	$(SRC)/$(GRID).cpp $(INCLUDE)/$(GRID).hpp $(COMMON_HDRS)
			$(COMPILE) $<

$(LIB)/$(CONTAINER).o:	$(SRC)/$(CONTAINER).cpp $(INCLUDE)/$(CONTAINER).hpp $(COMMON_HDRS)
			$(COMPILE) $<

run_all: run run_no_menu run_help run_invalid run_too_many

run:	$(TGT)
	./$<
	@#rmdir ~/.tsudoku

run_no_menu:	$(TGT)
		@#./$< --no-in-game-menu
		./$< -n

run_help:	$(TGT)
		-./$< --help

run_invalid:	$(TGT)
		-./$< --bad-flag

run_too_many:	$(TGT)
		-./$< --flag1 --flag2

clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)
