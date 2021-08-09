GCC = riscv32-unknown-elf-gcc
DUMP = riscv32-unknown-elf-objdump
LINK = riscv32-unknown-elf-gcc
COPY = riscv32-unknown-elf-objcopy

SRCDIR = ./src
OBJDIR = ./obj
HEADERDIR = ./src
# BINARY = lorawan_riscv
SOURCES = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(SRCDIR)/*.h)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

LDSCRIPT = link.riscv.ld
ELFFILE  = lorawan_riscv.elf
DUMPFILE = lorawan_riscv.dump
DUMPCOEFILE = $(addsuffix _dump.coe, $(basename $(DUMPFILE)))
S19FILE  = lorawan_riscv.s19
ICOEFILE = lorawan_riscv_instram.coe
DCOEFILE = lorawan_riscv_dataram.coe
MEMFILE = lorawan_riscv.mem

build: main.o $(ELFFILE)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(GCC) -c $< -o $@ 

main.o: main.c
	$(GCC) -c main.c -o main.o 

$(ELFFILE): $(OBJECTS)
	$(LINK) -o $(ELFFILE) main.o $(OBJECTS) -T $(LDSCRIPT) -nostartfiles -nostdlib

dump: $(ELFFILE)
	$(DUMP) -dC $(ELFFILE) > $(DUMPFILE)

dump_to_coe: $(DUMPFILE)
	perl ./scripts/convert_hex.pl $(DUMPFILE)
	mv $(addsuffix _hex, $(DUMPFILE)) $(DUMPCOEFILE)

coe: $(ELFFILE)
	$(COPY) -O srec $(ELFFILE) $(S19FILE)
	dos2unix $(S19FILE)
	perl ./scripts/convert_s19_to_coe.pl $(S19FILE)
	mv $(addsuffix _inst, $(S19FILE)) $(ICOEFILE)
	mv $(addsuffix _data, $(S19FILE)) $(DCOEFILE)

mem: $(S19FILE)
	perl ./scripts/convert_s19_to_mem.pl $(S19FILE)
	mv $(addsuffix _mem, $(S19FILE)) $(MEMFILE)

.PHONY: clean
clean:
	rm -rf lorawan_riscv* obj/*