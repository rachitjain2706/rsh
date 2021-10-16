.PHONY: all
all: 
	@ g++ -std=c++1y entry.cpp -o rsh

clean:
	@ rm rsh