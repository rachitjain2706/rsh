.PHONY: all
all: 
	@ clang++ -std=c++1y rsh.cc perform_commands.cc util.cc -o nyush

test:
	@ cp nyush grading && cd grading && ./gradeit.sh myoutputs nyush refout

clean:
	@ rm nyush