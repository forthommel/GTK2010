.PHONY: clean
READ_BIN=fread
WRITE_BIN=fwrite
CPP=g++
FLAGS=-std=c++0x -Wall

all: $(READ_BIN) $(WRITE_BIN) 

$(READ_BIN): fread.cpp
	$(CPP) $(FLAGS) $< -o $@

$(WRITE_BIN): fwrite.cpp
	$(CPP) $(FLAGS) $< -o $@

clean:
	rm -rf {$(READ_BIN),$(WRITE_BIN)}
