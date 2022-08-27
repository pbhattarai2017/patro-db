SRC := ./src
BUILD := ./build
OBJ := ./obj
MAIN := main

all: structure $(BUILD)/$(MAIN)

$(BUILD)/$(MAIN): $(OBJ)/$(MAIN).o
	$(CC) -o $(BUILD)/$(MAIN) $(OBJ)/$(MAIN).o `pkg-config --libs mysqlclient`

$(OBJ)/$(MAIN).o: $(SRC)/$(MAIN).c
	$(CC) `pkg-config --cflags mysqlclient` -o $(OBJ)/$(MAIN).o -c $(SRC)/$(MAIN).c

run: $(BUILD)/$(MAIN)
	$(BUILD)/$(MAIN)

clean: 
	rm $(OBJ)/$(MAIN).o
	rm $(BUILD)/$(MAIN)

structure:
	mkdir -p build
	mkdir -p obj
