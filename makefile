

test:
	g++ ./line_traverser.c ./draw_line.c ./line_bounder.c ./test_line_bounder.cpp ./tests.cpp --coverage -pthread -lgtest -g3 -o test -Wall -Wpedantic

clean:
	rm test *.gcno *.gcda
