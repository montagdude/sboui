CXX=g++
CXXFLAGS=-g -Wall
OBJ=ListBox.o MainWindow.o sboui.o
SRCDIR=src
CPPFLAGS=-Iinclude
EXE=sboui

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ) -lncurses

clean:
	rm *.o
	rm $(EXE)

ListBox.o: $(SRCDIR)/ListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ListBox.cpp

MainWindow.o: $(SRCDIR)/MainWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MainWindow.cpp

sboui.o: $(SRCDIR)/sboui.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sboui.cpp
