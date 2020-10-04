CC = gcc
CFLAGS = -g -Wall
LFLAGS = -lm -lSDL2 -lSDL2_image

NAME = raycast

SDIR = src
ODIR = obj
SRC = $(wildcard $(SDIR)/*.c)
OBJ = $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRC))

DEBUGGER = gdb -tui

default: $(NAME)

$(NAME): $(OBJ) 
	$(CC) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c 
	mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
.PHONY: run
.PHONY: debug

clean: 
	rm -rf $(ODIR)
	rm -f $(NAME)

run: $(NAME)
	./$(NAME)

debug: $(NAME)
	$(DEBUGGER) $(NAME)
