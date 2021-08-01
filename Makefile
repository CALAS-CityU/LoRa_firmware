CC = gcc
LD = gcc
SRCDIR = ./src
OBJDIR = ./obj
HEADERDIR = ./src
BINARY = lorawan_riscv
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))


all: compile link 

compile: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

link: $(BINARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BINARY)

clean:
	rm -rf lorawan_riscv* obj/*