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

vm: vm.c
	$(CC) $(CFLAGS) -o vm vm.c && ./vm

clean:
	rm -f $(TARGET) vm
