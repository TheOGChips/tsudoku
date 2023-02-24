# NOTE: The compiler to use
# TODO: Change this to g++ when the time comes
COMPILER = clang++

# NOTE: Everything needed to compile
INCLUDE_PATH = include
INCLUDE_FLAGS = -I $(INCLUDE_PATH)/ -std=c++17
COMPILE = $(COMPILER) -c $(INCLUDE_FLAGS)

# NOTE: Everything needed to link
LIB = lib
LINK_FLAGS = -lncurses
LINK = $(COMPILER) $(LINK_FLAGS) $(LIB)/*.o -o
VPATH = $(LIB)

# NOTE: Helpful common filepaths and filenames
SRC = src
MENU = Menu
MISC = misc
OBJ = $(MISC).o main.o $(MENU).o MainMenu.o SavedGameMenu.o DifficultyMenu.o InGameMenu.o Sudoku.o \
      Grid.o Container.o
COMMON_HDRS = $(INCLUDE_PATH)/colors.hpp $(INCLUDE_PATH)/$(MISC).hpp

# NOTE: The final executable name
TGT = tsudoku

# NOTE: Location to place a symbolic link to the final executable
SYMLINK_PATH = $(HOME)/.local/bin

.PHONY: all run_all run run_no_menu run_help run_invalid run_too_many clean uninstall purge

# NOTE: Compile and link
all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT)
	@mkdir -p $(SYMLINK_PATH)
	-@ln -s $(PWD)/$(TGT) $(SYMLINK_PATH)

%.o:	$(SRC)/%.cpp $(COMMON_HDRS)
		$(COMPILE) $<

$(MENU).o:	$(SRC)/$(MENU).cpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
		$(COMPILE) $<

%Menu.o:	$(SRC)/%Menu.cpp $(INCLUDE_PATH)/%Menu.hpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
		$(COMPILE) $<

# NOTE: Run all tests (except purge)
run_all: run run_no_menu run_help run_invalid run_too_many uninstall

# NOTE: Runs the executable with in-game menu enabled
run:	$(TGT)
	./$<

# NOTE: Runs the executable with in-game menu disabled
run_no_menu:	$(TGT)
		@#./$< --no-in-game-menu
		$< -n

# NOTE: Runs the executable and displays the help information
run_help:	$(TGT)
		$< --help

# NOTE: Tests the executable with a bad input option
run_invalid:	$(TGT)
		-$< --bad-flag

# NOTE: Tests the executable with too many (>1) input options
run_too_many:	$(TGT)
		-$< --flag1 --flag2

# NOTE: Removes all files and directories generated by this Makefile during compilation and linking
clean:	$(TGT)
	rm $< $(LIB)/*.o
	rmdir $(LIB)

# NOTE: Cleans and removes the symbolic link generated by this Makefile
uninstall:	clean
		@rm $(SYMLINK_PATH)/$(TGT)

# NOTE: Uninstalls and removes the hidden environment directory generated by the executable
purge:	uninstall
	rm -rf $(HOME)/.$(TGT)
