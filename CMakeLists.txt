# Set up project
cmake_minimum_required(VERSION 2.8.8)
set(SBOUI_VERSION 0.4.2)
project(sboui)

# Sources and include directories
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(sboui ${SOURCES})
set_property(TARGET sboui PROPERTY CXX_STANDARD 11)
set_property(TARGET sboui PROPERTY CXX_STANDARD_REQUIRED ON)

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
add_definitions(-DDATADIR=\"${DATADIR}\")

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
install(FILES etc/package_blacklist DESTINATION ${CONFDIR})
file(GLOB COLOR_THEMES etc/colors.*)
install(FILES ${COLOR_THEMES} DESTINATION ${DATADIR}/themes)
install(FILES man/sboui.8 DESTINATION ${MANDIR}/man8)
install(FILES man/sboui.conf.5 DESTINATION ${MANDIR}/man5)
