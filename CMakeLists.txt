# Set up project
cmake_minimum_required(VERSION 2.8.8)
set(SBOUI_VERSION 0.2)
project(sboui)

# Sources and include directories
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/Blacklist.cpp")

# Build Blacklist separately because it's the only thing that needs C++11
add_library(BLACKLIST OBJECT "src/Blacklist.cpp")
set_property(TARGET BLACKLIST PROPERTY CXX_STANDARD 11)
set_property(TARGET BLACKLIST PROPERTY CXX_STANDARD_REQUIRED ON)

# Create executable
add_executable(sboui $<TARGET_OBJECTS:BLACKLIST> ${SOURCES})

# Default compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Some default directories
set(CONFDIR /etc/sboui
    CACHE PATH "Directory to install sboui.conf."
)
set(DATADIR ${CMAKE_INSTALL_PREFIX}/share/sboui
    CACHE PATH "Directory to install shared data."
)
set(MANDIR ${CMAKE_INSTALL_PREFIX}/man
    CACHE PATH "Base directory to install manuals."
)
set(PACKAGEDIR /var/log/packages
    CACHE PATH "Location of installed packages list."
)

# Preprocessor definitions
add_definitions(-DPACKAGE_VERSION=\"${SBOUI_VERSION}\")
add_definitions(-DPACKAGE_DIR=\"${PACKAGEDIR}\")

# Check for curses library
find_package(Curses REQUIRED)
if (CURSES_FOUND)
  include_directories(${CURSES_INCLUDE_DIRS})
  target_link_libraries(sboui ${CURSES_LIBRARIES})
endif (CURSES_FOUND)

# Check for libconfig++
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Libconfig++ REQUIRED)
if (LIBCONFIG++_FOUND)
  include_directories(${LIBCONFIG++_INCLUDE_DIR})
  target_link_libraries(sboui ${LIBCONFIG++_LIBRARY})
endif (LIBCONFIG++_FOUND)

# Install rules
install(TARGETS sboui DESTINATION sbin)
install(FILES etc/sboui.conf DESTINATION ${CONFDIR})
file(GLOB COLOR_THEMES etc/colors.*)
install(FILES ${COLOR_THEMES} DESTINATION ${DATADIR})
install(FILES man/sboui.8 DESTINATION ${MANDIR}/man8)
install(FILES man/sboui.conf.5 DESTINATION ${MANDIR}/man5)
