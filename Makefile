CXX=g++
CXXFLAGS=-g -Wall
OBJ=Color.o settings.o string_util.o signals.o ListItem.o CategoryListItem.o BuildListItem.o sorting.o DirListing.o backend.o requirements.o CursesWidget.o AbstractListBox.o ListBox.o CategoryListBox.o BuildListBox.o SelectionBox.o FilterBox.o BuildActionBox.o DirListBox.o TagList.o BuildOrderBox.o InvReqBox.o InstallBox.o ScrollBox.o HelpItem.o HelpWindow.o InputItem.o TextInput.o ToggleInput.o Label.o InputBox.o SearchBox.o MessageBox.o filters.o MainWindow.o sboui.o
SRCDIR=src/cpp
CPPFLAGS=-Iinclude
EXE=sboui

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ) -lncurses -lconfig++

clean:
	rm *.o
	rm $(EXE)

Color.o: $(SRCDIR)/Color.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/Color.cpp

settings.o: $(SRCDIR)/settings.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/settings.cpp

string_util.o: $(SRCDIR)/string_util.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/string_util.cpp

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

CursesWidget.o: $(SRCDIR)/CursesWidget.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/CursesWidget.cpp

AbstractListBox.o: $(SRCDIR)/AbstractListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/AbstractListBox.cpp

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

DirListBox.o: $(SRCDIR)/DirListBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/DirListBox.cpp

TagList.o: $(SRCDIR)/TagList.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/TagList.cpp

BuildOrderBox.o: $(SRCDIR)/BuildOrderBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/BuildOrderBox.cpp

InvReqBox.o: $(SRCDIR)/InvReqBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InvReqBox.cpp

InstallBox.o: $(SRCDIR)/InstallBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InstallBox.cpp

ScrollBox.o: $(SRCDIR)/ScrollBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ScrollBox.cpp

HelpItem.o: $(SRCDIR)/HelpItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/HelpItem.cpp

HelpWindow.o: $(SRCDIR)/HelpWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/HelpWindow.cpp

InputItem.o: $(SRCDIR)/InputItem.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InputItem.cpp

TextInput.o: $(SRCDIR)/TextInput.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/TextInput.cpp

ToggleInput.o: $(SRCDIR)/ToggleInput.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/ToggleInput.cpp

Label.o: $(SRCDIR)/Label.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/Label.cpp

SearchBox.o: $(SRCDIR)/SearchBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/SearchBox.cpp

InputBox.o: $(SRCDIR)/InputBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/InputBox.cpp

MessageBox.o: $(SRCDIR)/MessageBox.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MessageBox.cpp

filters.o: $(SRCDIR)/filters.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/filters.cpp

MainWindow.o: $(SRCDIR)/MainWindow.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/MainWindow.cpp

sboui.o: $(SRCDIR)/sboui.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(SRCDIR)/sboui.cpp
