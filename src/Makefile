CXX = g++
FLEX = flex
BISON = bison

# Dir
INCLUDEDIR = ./include
BUILDDIR = ../build

CXXINCLUDE = -I./include -I.
CXXFLAGS = -Wno-write-strings -g ${CXXINCLUDE}
BISONFLAGS = -d -y

OBJS = main.o lexer.o parser.o AST.o symtab.o

TARGET = my_parser

all: $(TARGET)
	mv *.o ${BUILDDIR}/
	mv $(TARGET) ${BUILDDIR}/
	mv parser.tab.h ${BUILDDIR}/
	mv parser.tab.cc ${BUILDDIR}/
	mv lexer.yy.cc ${BUILDDIR}/

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cc ${INCLUDEDIR}/AST.h
	$(CXX) $(CXXFLAGS) -c main.cc

AST.o: AST.cc ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c AST.cc

symtab.o: symtab.cc ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c symtab.cc

lexer.o: lexer.l parser.o
	$(FLEX) -o lexer.yy.cc lexer.l
	$(CXX) $(CXXFLAGS) -c lexer.yy.cc -o lexer.o

parser.o: parser.y ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h
	$(BISON) $(BISONFLAGS) parser.y
	mv y.tab.c parser.tab.cc
	mv y.tab.h parser.tab.h
	$(CXX) $(CXXFLAGS) -c parser.tab.cc -o parser.o

clean:
	cd ${BUILDDIR} && rm -f $(TARGET) $(OBJS) parser.tab.cc parser.tab.h lexer.yy.cc

dotest:
	${BUILDDIR}/$(TARGET) ../test/1.say
