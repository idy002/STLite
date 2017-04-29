Test : code
#	time ./code
	time valgrind --leak-check=summary ./code
code : code.cpp deque.hpp
	g++ -g -o code --std=c++0x code.cpp
