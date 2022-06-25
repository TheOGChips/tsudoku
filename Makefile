SRC = src
MAIN = main
OBJ = $(MAIN).o
TGT = sudoku.exe

all:	$(OBJ)
		g++ -o $(TGT) $(OBJ)

$(MAIN).o:	$(SRC)/$(MAIN).cpp
			g++ -c $(SRC)/$(MAIN).cpp

.PHONY: run clean
run: $(TGT)
	 ./$(TGT)

clean:	$(TGT)
		rm $(TGT) $(OBJ)
