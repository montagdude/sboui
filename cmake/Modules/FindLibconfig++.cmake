find_path(LIBCONFIG++_INCLUDE_DIR libconfig.h++)
find_library(LIBCONFIG++_LIBRARY NAMES config++)

if (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)
  set(HAS_LIBCONFIG++ TRUE)
endif (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)

if (HAS_LIBCONFIG++)
  message(STATUS "Found libconfig++ header in ${LIBCONFIG++_INCLUDE_DIR}")
  message(STATUS "Found libconfig++ library: ${LIBCONFIG++_LIBRARY}")
else (HAS_LIBCONFIG++)
  if (not LIBCONFIG++_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find libconfig++.h!")
  endif (not LIBCONFIG++_INCLUDE_DIR)

  if (not LIBCONFIG++_LIBRARY)
    message(FATAL_ERROR "Could not find libconfig++ library!")
  endif (not LIBCONFIG++_LIBRARY)
endif (HAS_LIBCONFIG++)
