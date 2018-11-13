# Set up project
cmake_minimum_required(VERSION 2.8.8)
set(SBOUI_VERSION 2.0)
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
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fopenmp")

# Some default directories
set(CONFDIR /etc/sboui
    CACHE PATH "Directory to install sboui.conf and sboui-backend.conf."
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

# Variables related to the Exec command in the desktop launcher
set(GRAPHICAL_SU kdesu
    CACHE STRING "Graphical frontend for su (for desktop launcher)")
set(TERMINAL_EMULATOR xterm
    CACHE STRING "Terminal emulator (for desktop launcher)")

# kdesu requires -c to before command
if (${GRAPHICAL_SU} STREQUAL "kdesu")
    set(GRAPHICAL_SU_CMD "${GRAPHICAL_SU} -c")
else (${GRAPHICAL_SU} STREQUAL "kdesu")
    set(GRAPHICAL_SU_CMD "${GRAPHICAL_SU}")
endif (${GRAPHICAL_SU} STREQUAL "kdesu")

# rxvt and urxvt do not work with inner quotes
if (${TERMINAL_EMULATOR} STREQUAL "rxvt" OR
    ${TERMINAL_EMULATOR} STREQUAL "urxvt")
    set(BASH_SBOUI_CMD "/bin/bash -l -c /usr/sbin/sboui")
else (${TERMINAL_EMULATOR} STREQUAL "rxvt" OR
      ${TERMINAL_EMULATOR} STREQUAL "urxvt")
    set(BASH_SBOUI_CMD "'/bin/bash -l -c /usr/sbin/sboui'")
endif (${TERMINAL_EMULATOR} STREQUAL "rxvt" OR
       ${TERMINAL_EMULATOR} STREQUAL "urxvt")

# Preprocessor definitions
add_definitions(-DPACKAGE_VERSION=\"${SBOUI_VERSION}\")
add_definitions(-DCONFDIR=\"${CONFDIR}\")
add_definitions(-DPACKAGE_DIR=\"${PACKAGEDIR}\")
add_definitions(-DDATADIR=\"${DATADIR}\")

# Check for curses library
set(CURSES_NEED_WIDE TRUE)
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

# Configure sboui-backend and desktop launcher
configure_file(src/sboui-backend.in sboui-backend @ONLY)
configure_file(sboui.desktop.in sboui.desktop)

# Install rules
install(TARGETS sboui DESTINATION sbin)
install(FILES ${CMAKE_BINARY_DIR}/sboui-backend
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE
        DESTINATION sbin)
install(FILES etc/sboui.conf etc/sboui-backend.conf etc/package_blacklist
        DESTINATION ${CONFDIR})
file(GLOB COLOR_THEMES etc/colors.*)
install(FILES ${COLOR_THEMES} DESTINATION ${DATADIR}/themes)
install(FILES man/sboui.8 man/sboui-backend.8 DESTINATION ${MANDIR}/man8)
install(FILES man/sboui.conf.5 man/sboui-backend.conf.5
        DESTINATION ${MANDIR}/man5)
install(FILES ${CMAKE_BINARY_DIR}/sboui.desktop
       DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
