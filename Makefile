CXX=g++
CXXFLAGS=-g -Wall
OBJ=Color.o color_settings.o signals.o ListItem.o CategoryListItem.o BuildListItem.o sorting.o DirListing.o backend.o requirements.o ListBox.o CategoryListBox.o BuildListBox.o SelectionBox.o FilterBox.o BuildActionBox.o InputItem.o TextInput.o ToggleInput.o InputBox.o SearchBox.o MainWindow.o sboui.o
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

requirements.o: $(SRCDIR)/requirements.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/requirements.cpp

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

BuildActionBox.o: $(SRCDIR)/BuildActionBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/BuildActionBox.cpp

InputItem.o: $(SRCDIR)/InputItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InputItem.cpp

TextInput.o: $(SRCDIR)/TextInput.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/TextInput.cpp

ToggleInput.o: $(SRCDIR)/ToggleInput.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ToggleInput.cpp

SearchBox.o: $(SRCDIR)/SearchBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/SearchBox.cpp

InputBox.o: $(SRCDIR)/InputBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InputBox.cpp

MainWindow.o: $(SRCDIR)/MainWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MainWindow.cpp

sboui.o: $(SRCDIR)/sboui.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sboui.cpp
