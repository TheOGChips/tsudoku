COMPILER = clang++

INCLUDE_PATH = include
INCLUDE_FLAGS = -I $(INCLUDE_PATH)/ -std=c++17
COMPILE = $(COMPILER) -c $(INCLUDE_FLAGS)

LIB = lib
LINK_FLAGS = -lncurses
LINK = $(COMPILER) $(LINK_FLAGS) $(LIB)/*.o -o

SRC = src
MENU = Menu
SUDOKU = Sudoku
MISC = misc
OBJ = $(MISC).o main.o $(MENU).o MainMenu.o SavedGameMenu.o DifficultyMenu.o InGameMenu.o \
      $(SUDOKU).o Grid.o Container.o
COMMON_HDRS = $(INCLUDE_PATH)/colors.hpp $(INCLUDE_PATH)/$(MISC).hpp
# TODO: Fix this later
TGT = t$(SUDOKU)
SYMLINK_PATH = $(HOME)/.local/bin

.PHONY: all run_all run run_no_menu run_help run_invalid run_too_many clean uninstall purge

all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT)
	-@ln -s $(PWD)/$(TGT) $(SYMLINK_PATH)

%.o:	$(SRC)/%.cpp $(COMMON_HDRS)
	$(COMPILE) $<

$(MENU).o:	$(SRC)/$(MENU).cpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
	$(COMPILE) $<

%Menu.o:	$(SRC)/%Menu.cpp $(INCLUDE_PATH)/%Menu.hpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
	$(COMPILE) $<

run_all: run run_no_menu run_help run_invalid run_too_many uninstall

run:	$(TGT)
	./$<
	@#rmdir ~/.tsudoku

run_no_menu:	$(TGT)
		@#./$< --no-in-game-menu
		$< -n

run_help:	$(TGT)
		$< --help

run_invalid:	$(TGT)
		-$< --bad-flag

run_too_many:	$(TGT)
		-$< --flag1 --flag2

clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)
	
uninstall:	clean
		@rm $(SYMLINK_PATH)/$(TGT)

purge:	uninstall
	rm -rf $(HOME)/.tsudoku
