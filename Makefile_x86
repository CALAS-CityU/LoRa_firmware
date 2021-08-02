CC = gcc
LD = gcc
SRCDIR = ./src
OBJDIR = ./obj
HEADERDIR = ./src
BINARY = lorawan_riscv
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))


build: compile link 

compile: $(OBJECTS)
	$(CC) -o $@ $^ 

link: $(BINARY)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ 

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY)

clean:
	rm -rf lorawan_riscv* obj/*