PROG = NSixty4Save
OBJS = main.o

CFLAGS = -Wall -Werror

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS)

clean:
	rm -f $(PROG) $(OBJS)
