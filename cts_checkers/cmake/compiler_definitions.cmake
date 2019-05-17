# Only option-independent compiler definition are set here.
# Option-specific definitions reside in options.cmake.

# ----------------- GENERAL -----------------
# Ad-hoc configuration of internal libraries.
set(WITH_YAML          OFF)
set(MINRECTXT_PROTOBUF ON)

if (WITH_YAML)
  add_definitions(-DWITH_YAML)
endif()

if (MINRECTXT_PROTOBUF)
  add_definitions(-DMINRECTXT_PROTOBUF)
endif()

add_definitions(-DMINRECTXT_WITH_TOOLS)
# ------------- COMPILER SPECIFIC -----------

# Flags for MS Visual Studio.
IF (MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)

  # Classic workaround to prevent windows.h replace std::min/std::max with
  # its macros.
  ADD_DEFINITIONS(-DNOMINMAX)

  # Workaround MSVC bug with template specialization storage type.
  ADD_DEFINITIONS(-DUSE_STATIC_SPECIAL)

  # Disable inline assembler on x64 compiler.
  IF (CMAKE_CL_64)
    ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)
  ENDIF()

  # Disable inline assembler on ARM.
  IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm.*$")
    ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)
  ENDIF()

# It is GCC or Clang.
ELSEIF (CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_GNUC OR
        ${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR
        ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")

  # Disable MS-specific inline assembler.
  ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)

  # Make all libc functions re-entrant.
  ADD_DEFINITIONS(-D_REENTRANT)

  # Workaround for non-standard behaviour.
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  IF (GCC_VERSION VERSION_LESS 4.3)
    # Since 4.3 GCC conform to 7.1.1 of C++ standard disallowing
    # storage class on explicit template specialisations.
    ADD_DEFINITIONS(-DUSE_STATIC_SPECIAL)
  ENDIF()

  # Hide emscripten warnings
  IF (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths")
  ENDIF()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -march=native -mtune=native -g -ggdb -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wcast-align")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused -Wno-missing-field-initializers")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-cast-align")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

  IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm.*$")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
  ENDIF()

  # Debug
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb -O0 -g3 -fno-inline -fno-omit-frame-pointer")
  # Release
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
  # Release with debug info (for profile purposes).
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${CMAKE_C_FLAGS_RELEASE}")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -fno-omit-frame-pointer")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG")

  # CXX stuff is same as C stuff, plus something C++ specific.
  SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
  SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fexceptions")

# It is unknown. Do nothing.
ELSE()

  MESSAGE(WARNING "Unknown compiler. Not using any specific flags. The build will probably fail.")

ENDIF()

# --debug mode with extended visualization---
if (EXTENDED_DEBUG_MODE)
  add_definitions(-DEXTENDED_DEBUG_MODE)
endif()
