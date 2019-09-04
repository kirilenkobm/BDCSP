CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
RM = rm -f
TARGET = CSP
RND_GEN = generate_input

SRCS = src/CSP.c src/read_input.c src/patterns.c src/render.c src/traverse.c src/arrstuff.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${OBJS} $(SRCS:.c)

# to make inputs generator; standalone tool
.PHONY: rnd
rnd: $(RND_GEN)

$(RND_GEN): src/generate_input.c
	$(CC) $(CFLAGS) src/generate_input.c -o $(RND_GEN) $
