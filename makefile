
# Theres literally almost nothing here. What are you looking for?
# There are 2 cpp files, one is the useful draw line one, the other is tests.

test:
	g++ ./line_traverser.c ./draw_line.c ./tests.cpp --coverage -pthread -lgtest -g3 -o test -Wall -Wpedantic

clean:
	rm test *.gcno *.gcda
