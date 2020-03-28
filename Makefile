.PHONY: all clean

CC=g++
TARGET = scanner
CFLAGS= -std=c++14 -g -Wextra -static #-DLOW_MEMORY
LIBS=

IDIR =include
ODIR=obj
SDIR=src
ARCH_DIR=arch/aarch64
DISAS_DIR=disas

INCLUDE= -I$(ARCH_DIR) -I$(DISAS_DIR) -I$(IDIR)

_OBJECTS := Scanner.o Handlers.o ScannerManager.o ScannerManagerMC.o ArchProperties.o ArchFunctions.o Utility.o Analyser.o Feeder.o Blacklist.o ScannerManagerPtrace.o
OBJECTS = $(patsubst %,$(ODIR)/%,$(_OBJECTS))

_DEPS = Scanner.h Handlers.h ScannerManager.h ScannerManagerMC.h Utility.h Analyser.h Feeder.h Blacklist.h ScannerManagerPtrace.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS)) $(ARCH_DIR)/ArchProperties.h $(ARCH_DIR)/ArchFunctions.h

_DISAS = capstone/libcapstone.a RV-disas/RV-disas.a
DISAS = $(patsubst %,$(DISAS_DIR)/%,$(_DISAS))

all: $(TARGET)

$(TARGET): $(SDIR)/main.cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $< $(OBJECTS) $(DISAS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $(INCLUDE)  $<  $(LIBS) -o $@

$(ODIR)/ArchProperties.o: $(ARCH_DIR)/ArchProperties.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< $(LIBS) -o $@

$(ODIR)/ArchFunctions.o: $(ARCH_DIR)/ArchFunctions.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< $(LIBS) -o $@





clean:
	rm -f $(ODIR)/*.o
	rm -f $(TARGET)
