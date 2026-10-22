CC     = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC    = src/main.c src/operations.c
TARGET = browser

build: $(SRC) include/functions.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: build
	./$(TARGET) examples/02-navigation/input.txt output.txt
	@cat output.txt

test: build
	@pass=0; fail=0; \
	for d in examples/*/; do \
	    ./$(TARGET) $$d/input.txt output.txt > /dev/null 2>&1; \
	    if diff -q $$d/expected.txt output.txt > /dev/null; then \
	        echo "PASS $$d"; pass=$$((pass+1)); \
	    else \
	        echo "FAIL $$d"; fail=$$((fail+1)); \
	    fi; \
	done; \
	rm -f output.txt; \
	echo "$$pass passed, $$fail failed"; \
	[ $$fail -eq 0 ]

clean:
	rm -f $(TARGET) output.txt
