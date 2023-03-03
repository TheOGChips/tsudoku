# NOTE: The compiler to use
COMPILER = g++

# NOTE: Everything needed to compile
INCLUDE_PATH = include
INCLUDE_FLAGS = -I $(INCLUDE_PATH)/ -std=c++17
COMPILE = $(COMPILER) -c $(INCLUDE_FLAGS)

# NOTE: Everything needed to link
LIB = lib
LINK = $(COMPILER) $(LIB)/*.o -o
VPATH = $(LIB)

# NOTE: Helpful common filepaths and filenames
SRC = src
MENU = Menu
MISC = misc
OBJ = $(TGT).o $(MISC).o main.o $(MENU).o MainMenu.o SavedGameMenu.o DifficultyMenu.o InGameMenu.o \
      Sudoku.o Grid.o Container.o
COMMON_HDRS = $(INCLUDE_PATH)/colors.hpp $(INCLUDE_PATH)/$(MISC).hpp

# NOTE: The final executable name
TGT = tsudoku

# NOTE: Location to place a symbolic link to the final executable
SYMLINK_PATH = $(HOME)/.local/bin

.PHONY: all clean uninstall purge upgrade

# NOTE: Compile and link
all:	$(OBJ)
	mkdir -p $(LIB)
	-mv $(?F) $(LIB)/
	$(LINK) $(TGT) -lncurses
	@mkdir -p $(SYMLINK_PATH)
	-@ln -s $(PWD)/$(TGT) $(SYMLINK_PATH)
	@echo -e '\nInstallation of tsudoku has completed!'
	@echo "Remember to either open a new terminal window or source your shell's RC file"
	@echo -e 'in order to run "tsudoku" in the current window\n'

%.o:	$(SRC)/%.cpp $(COMMON_HDRS)
		$(COMPILE) $<

$(MENU).o:	$(SRC)/$(MENU).cpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
		$(COMPILE) $<

%Menu.o:	$(SRC)/%Menu.cpp $(INCLUDE_PATH)/%Menu.hpp $(INCLUDE_PATH)/$(MENU).hpp $(COMMON_HDRS)
		$(COMPILE) $<

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

# NOTE: Upgrades the current installation of tsudoku
upgrade:	uninstall
		$(MAKE) all
