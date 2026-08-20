CC 	:= gcc

CFLAGS	:= -D_POSIX_C_SOURCE=200809L \
		-Wall \
		-Wextra \
		-std=c11 \
		-Isrc

SRC	:= \
	src/main.c \
	src/pomodoro.c \
	src/timer.c \
	src/ui.c \
	src/ascii.c

OBJ	:= $(SRC:.c=.o)

TARGET	:= build/tick

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -lncursesw -o $(TARGET)

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
