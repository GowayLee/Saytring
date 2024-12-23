CXX = g++
FLEX = flex
BISON = bison

# Dir
INCLUDEDIR = ./include
BUILDDIR = ../build

CXXINCLUDE = -I./include -I.
CXXFLAGS = -Wno-write-strings -g ${CXXINCLUDE}
BISONFLAGS = -d -y

OBJS = main.o lexer.o parser.o symtab.o util.o semant.o cgen.o core_func.o flag_handler.o

TARGET = saytringc

all: $(TARGET)

build: $(TARGET)
	mv *.o ${BUILDDIR}/
	mv $(TARGET) ${BUILDDIR}/
	mv parser.tab.h ${BUILDDIR}/
	mv parser.tab.cc ${BUILDDIR}/
	mv lexer.yy.cc ${BUILDDIR}/

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cc ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/flag_handler.h parser.tab.h
	$(CXX) $(CXXFLAGS) -c main.cc

flag_handler.o: ${INCLUDEDIR}/flag_handler.h
	$(CXX) $(CXXFLAGS) -c flag_handler.cc

cgen.o: cgen.cc ${INCLUDEDIR}/cgen.h ${INCLUDEDIR}/core_func.h ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h ${INCLUDEDIR}/template.h
	$(CXX) $(CXXFLAGS) -c cgen.cc

semant.o: semant.cc parser.tab.h ${INCLUDEDIR}/core_func.h ${INCLUDEDIR}/semant.h ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c semant.cc

core_func.o: core_func.cc ${INCLUDEDIR}/core_func.h ${INCLUDEDIR}/semant.h ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c core_func.cc

symtab.o: symtab.cc ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c symtab.cc

util.o: util.cc parser.tab.h ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h
	$(CXX) $(CXXFLAGS) -c util.cc

lexer.o: lexer.l parser.tab.h
	$(FLEX) -o lexer.yy.cc lexer.l
	$(CXX) $(CXXFLAGS) -c lexer.yy.cc -o lexer.o

parser.o: parser.tab.h parser.tab.cc ${INCLUDEDIR}/AST.h ${INCLUDEDIR}/symtab.h ${INCLUDEDIR}/util.h
	$(CXX) $(CXXFLAGS) -c parser.tab.cc -o parser.o

parser.tab.h parser.tab.cc: parser.y
	$(BISON) $(BISONFLAGS) parser.y
	mv y.tab.c parser.tab.cc
	mv y.tab.h parser.tab.h

clean:
	rm -f $(TARGET) $(OBJS) parser.tab.cc parser.tab.h lexer.yy.cc *.py

dotest:
	${BUILDDIR}/$(TARGET) ../test/sin.say || ./$(TARGET) ../test/sin.say
