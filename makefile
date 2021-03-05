
# Theres literally almost nothing here. What are you looking for?
# There are 2 cpp files, one is the useful draw line one, the other is tests.

test:
	g++ ./draw_line.cpp ./tests.cpp --coverage -pthread -lgtest -g3 -o test
