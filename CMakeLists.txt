# Set up project
cmake_minimum_required(VERSION 2.8.8)
set(SBOUI_VERSION 2.0.1)
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
set(PACKAGEDIR /var/lib/pkgtools/packages
    CACHE PATH "Location of installed packages list."
)

set(CRONDIR /etc/cron.hourly
    CACHE PATH "Directory for sboui --sync cronjob (only if INSTALL_SYSTRAY_NOTIFIER is enabled)."
)

# Variables related to the Exec command in the desktop launcher
set(GRAPHICAL_SU kdesu
    CACHE STRING "Graphical frontend for su (for desktop launcher)")
set(TERMINAL_EMULATOR xterm
    CACHE STRING "Terminal emulator (for desktop launcher)")

# Whether to install --sync cronjob and systray update notifier
set(INSTALL_SYSTRAY_NOTIFIER FALSE
    CACHE BOOL "Whether to install --sync cronjob and systray update notifier")

# kdesu requires -c to before command
if (${GRAPHICAL_SU} STREQUAL "kdesu")
    set(GRAPHICAL_SU_CMD "${GRAPHICAL_SU} -c")
else (${GRAPHICAL_SU} STREQUAL "kdesu")
    set(GRAPHICAL_SU_CMD "${GRAPHICAL_SU}")
endif (${GRAPHICAL_SU} STREQUAL "kdesu")

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

# Configure files
configure_file(src/sboui-backend.in sboui-backend @ONLY)
configure_file(src/sboui_launch.in sboui_launch @ONLY)
configure_file(src/sboui_update_launch.in sboui_update_launch @ONLY)
configure_file(src/sboui-update-notifier.in sboui-update-notifier)
configure_file(sboui.desktop.in sboui.desktop)
configure_file(etc/sboui-sync.py.in sboui-sync.py)

# Install rules
install(TARGETS sboui DESTINATION sbin)
install(FILES ${CMAKE_BINARY_DIR}/sboui-backend
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE
        DESTINATION sbin)
install(FILES ${CMAKE_BINARY_DIR}/sboui_launch
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE
        DESTINATION libexec/sboui)
install(FILES ${CMAKE_BINARY_DIR}/sboui_update_launch
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE
        DESTINATION libexec/sboui)
install(FILES etc/sboui.conf etc/sboui-backend.conf etc/package_blacklist
        DESTINATION ${CONFDIR})
file(GLOB COLOR_THEMES etc/colors.*)
install(FILES ${COLOR_THEMES} DESTINATION ${DATADIR}/themes)
install(FILES man/sboui.8 man/sboui-backend.8 DESTINATION ${MANDIR}/man8)
install(FILES man/sboui.conf.5 man/sboui-backend.conf.5
        DESTINATION ${MANDIR}/man5)
install(FILES ${CMAKE_BINARY_DIR}/sboui.desktop
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES icons/sboui.svg
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)

# The following are only installed if INSTALL_SYSTRAY_NOTIFIER is enabled
if(INSTALL_SYSTRAY_NOTIFIER)
    install(FILES ${CMAKE_BINARY_DIR}/sboui-update-notifier
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ             GROUP_EXECUTE
                        WORLD_READ             WORLD_EXECUTE
            DESTINATION bin)
    install(FILES src/sboui-systray.py
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ             GROUP_EXECUTE
                        WORLD_READ             WORLD_EXECUTE
            DESTINATION libexec/sboui)
    install(FILES ${CMAKE_BINARY_DIR}/sboui-sync.py
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ             GROUP_EXECUTE
                        WORLD_READ             WORLD_EXECUTE
            DESTINATION ${CRONDIR})
    install(FILES man/sboui-update-notifier.1
            DESTINATION ${MANDIR}/man1)
endif(INSTALL_SYSTRAY_NOTIFIER)
