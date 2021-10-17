.PHONY: all
all: 
	@ g++ -std=c++1y entry.cpp -o nyush

clean:
	@ rm nyush