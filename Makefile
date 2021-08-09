GCC = riscv32-unknown-elf-gcc
DUMP = riscv32-unknown-elf-objdump
LINK = riscv32-unknown-elf-gcc
COPY = riscv32-unknown-elf-objcopy
#LINK = riscv32-unknown-elf-ld
SRC = ./src
OBJ = ./obj
SRCFILES = $(SRC)/aes.c  $(SRC)/LoRaWan.c $(SRC)/sleep.c  \
	$(SRC)/LoRaMacCrypto.c   $(SRC)/utilities.c \
	$(SRC)/cmac.c $(SRC)/io.c $(SRC)/main.c $(SRC)/sx1262.c $(SRC)/sc_print.c $(SRC)/curve25519-donna.c
HDRFILES = $(SRC)/cmac.h $(SRC)/io.h $(SRC)/sx1262.h $(SRC)/aes.h \
	$(SRC)/glb_include.h $(SRC)/LoRaWan.h $(SRC)/sleep.h $(SRC)/LoRaMacCrypto.h $(SRC)/utilities.h $(SRC)/sc_print.h $(SRC)/ecdh.h
OBJFILES = $(OBJ)/main.o $(OBJ)/utilities.o $(OBJ)/io.o $(OBJ)/sleep.o $(OBJ)/aes.o $(OBJ)/cmac.o $(OBJ)/LoRaMacCrypto.o \
	$(OBJ)/sx1262.o $(OBJ)/LoRaWan.o $(OBJ)/sc_print.o $(OBJ)/curve25519-donna.o $(OBJ)/ecdh.o
LDSCRIPT = link.riscv.ld

ELFFILE  = lorawan_riscv.elf

DUMPFILE = lorawan_riscv.dump
DUMPCOEFILE = $(addsuffix _dump.coe, $(basename $(DUMPFILE)))

S19FILE  = lorawan_riscv.s19
ICOEFILE = lorawan_riscv_instram.coe
DCOEFILE = lorawan_riscv_dataram.coe

MEMFILE = lorawan_riscv.mem

build: compile link

link: $(OBJFILES)
	$(LINK) -o $(ELFFILE) $(OBJFILES) -T $(LDSCRIPT) -nostartfiles -nostdlib

compile: $(SRCFILES) $(HDRFILES)
	$(GCC) -c $(SRC)/main.c -o $(OBJ)/main.o
	$(GCC) -c $(SRC)/LoRaWan.c -o $(OBJ)/LoRaWan.o
	$(GCC) -c $(SRC)/sx1262.c -o $(OBJ)/sx1262.o
	$(GCC) -c $(SRC)/LoRaMacCrypto.c -o $(OBJ)/LoRaMacCrypto.o
	$(GCC) -c $(SRC)/cmac.c -o $(OBJ)/cmac.o
	$(GCC) -c $(SRC)/aes.c -o $(OBJ)/aes.o
	$(GCC) -c $(SRC)/utilities.c -o $(OBJ)/utilities.o
	$(GCC) -c $(SRC)/io.c -o $(OBJ)/io.o
	$(GCC) -c $(SRC)/sleep.c -o $(OBJ)/sleep.o
	$(GCC) -c $(SRC)/sc_print.c -o $(OBJ)/sc_print.o
	$(GCC) -c $(SRC)/curve25519-donna.c -o $(OBJ)/curve25519-donna.o
	$(GCC) -c $(SRC)/ecdh.c -o $(OBJ)/ecdh.o
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

