CC = clang
CFLAGS = -g -fsanitize=address
SRC = main.c lex.c parse.c ast_walking.c hashmap.c utils.c
TARGET = schemelike
EXAMPLE_FILE = example.scm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET) $(EXAMPLE_FILE)

clean:
	rm -f $(TARGET)
