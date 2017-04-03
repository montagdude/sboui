find_path(LIBCONFIG++_INCLUDE_DIR libconfig.h++)
find_library(LIBCONFIG++_LIBRARY NAMES config++)

if (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)
  set(LIBCONFIG++_FOUND TRUE)
endif (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)

if (LIBCONFIG++_FOUND)
  message(STATUS "Found libconfig++ header in ${LIBCONFIG++_INCLUDE_DIR}")
  message(STATUS "Found libconfig++ library: ${LIBCONFIG++_LIBRARY}")
else (LIBCONFIG++_FOUND)
  if (NOT LIBCONFIG++_INCLUDE_DIR)
    message(FATAL_ERROR "Could NOT find libconfig++.h!")
  endif (NOT LIBCONFIG++_INCLUDE_DIR)

  if (NOT LIBCONFIG++_LIBRARY)
    message(FATAL_ERROR "Could NOT find libconfig++ library!")
  endif (NOT LIBCONFIG++_LIBRARY)
endif (LIBCONFIG++_FOUND)
