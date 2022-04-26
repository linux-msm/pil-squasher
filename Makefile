OUT := pil-squasher

CFLAGS ?= -Wall -g -O2
LDFLAGS ?=
PREFIX ?= /usr/local

SRCS := pil-squasher.c
OBJS := $(SRCS:.c=.o)

$(OUT): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

install: $(OUT)
	install -D -m 755 $< $(DESTDIR)$(PREFIX)/bin/$<

clean:
	rm -f $(OUT) $(OBJS)

