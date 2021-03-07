
# Theres literally almost nothing here. What are you looking for?
# There are 2 cpp files, one is the useful draw line one, the other is tests.

test: line_traverser.cpp line_traverser.h draw_line.cpp draw_line.h test.cpp
	g++ ./line_traverser.cpp ./draw_line.cpp ./tests.cpp --coverage -pthread -lgtest -g3 -o test -Wall -Wpedantic

clean:
	rm test *.gcno *.gcda
