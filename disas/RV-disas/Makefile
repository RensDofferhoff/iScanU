RISCV_DISAS_OBJ = RV-disas.o
RISCV_DISAS_LIB = RV-disas.a

CFLAGS = -O2

all: $(RISCV_DISAS_LIB)

clean:
	rm -f *.o
	rm -f *.a

RV-disas.a: $(RISCV_DISAS_OBJ)
	$(AR) cr $@ $^
	rm -f RV-disas.o

RV-disas.o: riscv-disas.c
	g++ $(CFLAGS) -c $^ -o $@
