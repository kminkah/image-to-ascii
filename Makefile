# compilation
CC = gcc
CFLAGS = -Wall -Wextra -g -I$(INCDIR)

# directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# target
TARGET = $(BINDIR)/program

all: $(TARGET)

# linking
$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# print
print:
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Target binary: $(TARGET)"

# PHONY
.PHONY: all clean print

