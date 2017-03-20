# Set up project
cmake_minimum_required(VERSION 3.5.2)
set(SBOUI_VERSION 0.1)
project(sboui)

# Sources and include directories
include_directories(include)
file(GLOB SOURCES "src/cpp/*.cpp")
add_executable(sboui ${SOURCES})

# Default compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Some default install directories
set(CONFDIR /etc/sboui
    CACHE PATH "Directory to install sboui.cfg."
)
set(LIBEXECDIR ${CMAKE_INSTALL_PREFIX}/libexec
    CACHE PATH "Directory to install sboutil.sh."
)
set(DATADIR ${CMAKE_INSTALL_PREFIX}/share
    CACHE PATH "Directory to install shared data."
)
set(MANDIR ${CMAKE_INSTALL_PREFIX}/man
    CACHE PATH "Base directory to install manuals."
)

# Preprocessor definitions
add_definitions(-DPACKAGE_VERSION=\"${SBOUI_VERSION}\")
add_definitions(-DLIBEXECDIR=\"${LIBEXECDIR}\")

# Check for curses library
find_package(Curses REQUIRED)
if (CURSES_FOUND)
  include_directories(${CURSES_INCLUDE_DIRS})
  target_link_libraries(sboui ${CURSES_LIBRARIES})
endif (CURSES_FOUND)

# Check for libconfig++
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Config++ REQUIRED)
if (CONFIG++_FOUND)
  include_directories(${CONFIG++_INCLUDE_DIR})
  target_link_libraries(sboui ${CONFIG++_LIBRARY})
endif (CONFIG++_FOUND)

# Install rules
install(TARGETS sboui DESTINATION sbin)
install(FILES etc/sboui.cfg DESTINATION ${CONFDIR})
install(FILES src/shell/sboutil.sh 
              DESTINATION ${LIBEXECDIR}
              PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                          GROUP_READ             GROUP_EXECUTE
                          WORLD_READ             WORLD_EXECUTE
)
file(GLOB COLOR_THEMES etc/colors_*.cfg)
install(FILES ${COLOR_THEMES} DESTINATION ${DATADIR})
install(FILES man/sboui.8 DESTINATION ${MANDIR}/man8)
install(FILES man/sboui.cfg.5 DESTINATION ${MANDIR}/man5)
