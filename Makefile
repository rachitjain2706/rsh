.PHONY: all
all: 
	@ g++ entry.cpp -o rsh

clean:
	@ rm rsh