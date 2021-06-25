project(TerminalClipNative)
cmake_minimum_required(VERSION 3.20)
set(EMACS_EXEC_PATH "emacs" CACHE STRING "Sets the path for the Emacs installation, for automatic module file suffix discovery.")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)

execute_process(COMMAND "${EMACS_EXEC_PATH}" --batch --eval "(princ module-file-suffix)"
  OUTPUT_VARIABLE EMACS_MODULE_FILE_SUFFIX
  COMMAND_ERROR_IS_FATAL ANY)

add_subdirectory(clip)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(terminal-clip-native SHARED module.cpp)

if ("${EMACS_MODULE_FILE_SUFFIX}" EQUAL nil)
  message("Given Emacs installation wasn't compiled with dynamic module support. Couldn't determine target module extension. Falling back to default.")
else()
  set_target_properties(terminal-clip-native PROPERTIES
    PREFIX ""
    SUFFIX "${EMACS_MODULE_FILE_SUFFIX}")
endif()

target_link_libraries(terminal-clip-native clip)
