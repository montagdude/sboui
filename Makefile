CXX=clang++
CXXFLAGS=-g -Wall
OBJ=Color.o color_settings.o signals.o ListItem.o CategoryListItem.o BuildListItem.o sorting.o DirListing.o backend.o ListBox.o CategoryListBox.o BuildListBox.o SelectionBox.o FilterBox.o InputBox.o MainWindow.o sboui.o
SRCDIR=src/cpp
CPPFLAGS=-Iinclude
EXE=sboui

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ) -lncurses

clean:
	rm *.o
	rm $(EXE)

Color.o: $(SRCDIR)/Color.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/Color.cpp

color_settings.o: $(SRCDIR)/color_settings.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/color_settings.cpp

signals.o: $(SRCDIR)/signals.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/signals.cpp

ListItem.o: $(SRCDIR)/ListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ListItem.cpp

CategoryListItem.o: $(SRCDIR)/CategoryListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/CategoryListItem.cpp

BuildListItem.o: $(SRCDIR)/BuildListItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/BuildListItem.cpp

sorting.o: $(SRCDIR)/sorting.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sorting.cpp

DirListing.o: $(SRCDIR)/DirListing.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/DirListing.cpp

backend.o: $(SRCDIR)/backend.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/backend.cpp

ListBox.o: $(SRCDIR)/ListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ListBox.cpp

CategoryListBox.o: $(SRCDIR)/CategoryListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/CategoryListBox.cpp

BuildListBox.o: $(SRCDIR)/BuildListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/BuildListBox.cpp

SelectionBox.o: $(SRCDIR)/SelectionBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/SelectionBox.cpp

FilterBox.o: $(SRCDIR)/FilterBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/FilterBox.cpp

InputBox.o: $(SRCDIR)/InputBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InputBox.cpp

MainWindow.o: $(SRCDIR)/MainWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MainWindow.cpp

sboui.o: $(SRCDIR)/sboui.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sboui.cpp
