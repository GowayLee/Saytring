CXX = g++
FLEX = flex
BISON = bison

# Directories
INCLUDEDIR = ./include
BUILDDIR = ../build

# Compiler Flags
CXXINCLUDE = -I./include -I. -I$(BUILDDIR)
CXXFLAGS = -std=c++17 -Wno-write-strings -g ${CXXINCLUDE}
BISONFLAGS = -d -y

# Object Files
OBJS = cgen.o core_func.o main.o semant.o symtab.o util.o parser.o lexer.o

# Target Executable
TARGET = saytringc

# Default Target
all: parser.tab.h lexer.yy.cc $(TARGET)

# Build Target
build: $(TARGET)
	mkdir -p ${BUILDDIR}
	mv $(TARGET) ${BUILDDIR}/
	mv *.o ${BUILDDIR}/
	mv lexer.yy.cc ${BUILDDIR}/
	mv parser.tab.h ${BUILDDIR}/
	mv parser.tab.cc ${BUILDDIR}/

# Linker Step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compilation Rules
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

lexer.o: lexer.yy.cc
	$(CXX) $(CXXFLAGS) -c lexer.yy.cc -o lexer.o

parser.o: parser.tab.cc parser.tab.h
	$(CXX) $(CXXFLAGS) -c parser.tab.cc -o parser.o

lexer.yy.cc: lexer.l
	$(FLEX) -o lexer.yy.cc lexer.l

parser.tab.h parser.tab.cc: parser.y
	$(BISON) $(BISONFLAGS) parser.y
	mv y.tab.c parser.tab.cc
	mv y.tab.h parser.tab.h

# Cleaning Rules
clean:
	rm -f $(OBJS) $(TARGET) parser.tab.* lexer.yy.cc
	rm -rf $(BUILDDIR)

# Test Script
dotest:
	for testfile in ../test/*.say; do \
		echo "Running $$testfile..."; \
		$(BUILDDIR)/$(TARGET) $$testfile || ./$(TARGET) $$testfile; \
	done