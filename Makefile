OUT := pil-squasher pil-splitter

CFLAGS ?= -Wall -g -O2
LDFLAGS ?=
prefix ?= /usr/local

SRCS := pil-squasher.c pil-splitter.c
OBJS := $(SRCS:.c=.o)

all: $(OUT)

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^

install: $(OUT)
	install -D -m 755 $< $(DESTDIR)$(prefix)/bin/$<

clean:
	rm -f $(OUT) $(OBJS)

