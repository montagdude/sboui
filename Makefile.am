sbin_PROGRAMS = sboui

sboui_SOURCES = include/Color.h \
                include/settings.h \
                include/string_util.h \
                include/signals.h \
                include/ListItem.h \
                include/CategoryListItem.h \
                include/BuildListItem.h  \
                include/sorting.h  \
                include/DirListing.h  \
                include/backend.h  \
                include/requirements.h \
                include/CursesWidget.h \
                include/AbstractListBox.h \
                include/ListBox.h \
                include/CategoryListBox.h \
                include/BuildListBox.h \
                include/SelectionBox.h \
                include/FilterBox.h \
                include/BuildActionBox.h \
                include/DirListBox.h \
                include/TagList.h \
                include/BuildOrderBox.h \
                include/InvReqBox.h \
                include/InstallBox.h \
                include/ScrollBox.h \
                include/HelpItem.h \
                include/HelpWindow.h \
                include/InputItem.h \
                include/TextInput.h \
                include/ToggleInput.h \
                include/Label.h \
                include/ComboBoxList.h \
                include/ComboBox.h \
                include/InputBox.h \
                include/SearchBox.h \
                include/OptionsWindow.h \
                include/MessageBox.h \
                include/filters.h \
                include/MainWindow.h \
                src/cpp/Color.cpp \
                src/cpp/settings.cpp \
                src/cpp/string_util.cpp \
                src/cpp/signals.cpp \
                src/cpp/ListItem.cpp \
                src/cpp/CategoryListItem.cpp \
                src/cpp/BuildListItem.cpp \
                src/cpp/sorting.cpp \
                src/cpp/DirListing.cpp \
                src/cpp/backend.cpp \
                src/cpp/requirements.cpp \
                src/cpp/CursesWidget.cpp \
                src/cpp/AbstractListBox.cpp \
                src/cpp/ListBox.cpp \
                src/cpp/CategoryListBox.cpp \
                src/cpp/BuildListBox.cpp \
                src/cpp/SelectionBox.cpp \
                src/cpp/FilterBox.cpp \
                src/cpp/BuildActionBox.cpp \
                src/cpp/DirListBox.cpp \
                src/cpp/TagList.cpp \
                src/cpp/BuildOrderBox.cpp \
                src/cpp/InvReqBox.cpp \
                src/cpp/InstallBox.cpp \
                src/cpp/ScrollBox.cpp \
                src/cpp/HelpItem.cpp \
                src/cpp/HelpWindow.cpp \
                src/cpp/InputItem.cpp \
                src/cpp/TextInput.cpp \
                src/cpp/ToggleInput.cpp \
                src/cpp/Label.cpp \
                src/cpp/ComboBoxList.cpp \
                src/cpp/ComboBox.cpp \
                src/cpp/InputBox.cpp \
                src/cpp/SearchBox.cpp \
                src/cpp/OptionsWindow.cpp \
                src/cpp/MessageBox.cpp \
                src/cpp/filters.cpp \
                src/cpp/MainWindow.cpp \
                src/cpp/sboui.cpp 

sboui_CPPFLAGS = -I$(top_srcdir)/include -DINSTALL_PREFIX=\"$(prefix)\" -DPKGLIBEXECDIR=\"$(pkglibexecdir)\" -DSYSCONFDIR=\"$(sysconfdir)\"
sboui_CXXFLAGS = -g -Wall
sboui_LDFLAGS = -lncurses -lconfig++

pkglibexec_SCRIPTS = src/shell/sboutil.sh
sysconf_DATA = etc/sboui.cfg
dataroot_DATA = etc/colors_dark.cfg etc/colors_light.cfg
