CXX=g++
CXXFLAGS=-g -Wall
OBJ=ListItem.o CategoryListItem.o SlackBuildListItem.o ListBox.o CategoryListBox.o MainWindow.o sboui.o
SRCDIR=src
CPPFLAGS=-Iinclude
EXE=sboui

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ) -lncurses

clean:
	rm *.o
	rm $(EXE)

ListItem.o: $(SRCDIR)/ListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ListItem.cpp

CategoryListItem.o: $(SRCDIR)/CategoryListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/CategoryListItem.cpp

SlackBuildListItem.o: $(SRCDIR)/SlackBuildListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/SlackBuildListItem.cpp

ListBox.o: $(SRCDIR)/ListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ListBox.cpp

CategoryListBox.o: $(SRCDIR)/CategoryListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/CategoryListBox.cpp

MainWindow.o: $(SRCDIR)/MainWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MainWindow.cpp

sboui.o: $(SRCDIR)/sboui.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sboui.cpp
