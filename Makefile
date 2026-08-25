PREFIX	?= /usr/local
BINDIR	?= $(PREFIX)/bin
DATADIR	?= $(PREFIX)/share/tick
CC 	:= gcc

CFLAGS	:= -D_POSIX_C_SOURCE=200809L \
		-Wall \
		-Wextra \
		-Werror \
		-pedantic \
		-std=c11 \
		-Isrc

SRC	:= \
	src/main.c \
	src/pomodoro.c \
	src/timer.c \
	src/ui.c \
	src/ascii.c \
	src/audio.c 

OBJ	:= $(SRC:.c=.o)

TARGET	:= build/tick

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) -lncursesw -o $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/tick
	install -d $(DESTDIR)$(DATADIR)/sounds
	install -m 644 sounds/*.wav $(DESTDIR)$(DATADIR)/sounds/


uninstall:
	rm -f $(DESTDIR)$(BINDIR)/tick
	rm -rf $(DESTDIR)$(DATADIR)

.PHONY: clean run install uninstall

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
