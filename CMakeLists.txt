cmake_minimum_required(VERSION 3.0)

option(WIN64CC "Cross-compile for Windows x64 using MinGW-w64" OFF)

if(WIN64CC)
  set(CMAKE_SYSTEM_NAME Windows)
  set(CMAKE_SYSTEM_PROCESSOR x86_64)
  set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
  set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
  set(CMAKE_RC_COMPILER /usr/bin/x86_64-w64-mingw32-windres)
  set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

project(asteroids-jam C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG master)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
endif()

FetchContent_Declare(visit_struct
  GIT_REPOSITORY https://github.com/cbeck88/visit_struct.git
  GIT_TAG v1.0
  CONFIGURE_COMMAND ""
  BUILD_COMMAND "")
FetchContent_GetProperties(visit_struct)
if(NOT visit_struct_POPULATED)
  FetchContent_Populate(visit_struct)
endif()

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp") 
file(GLOB_RECURSE PROJECT_DATA CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/data/*")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") 

# Declaring our executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE} ${visit_struct_SOURCE_DIR}/include/ ${json_SOURCE_DIR}/single_include/)

add_custom_target(copy_data ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
  DEPENDS ${PROJECT_DATA})
add_dependencies(${PROJECT_NAME} copy_data)

if (EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME}
          PROPERTIES SUFFIX ".html"
          LINK_FLAGS " --bind -s WASM=1 -s MIN_WEBGL_VERSION=1 -s ABORT_ON_WASM_EXCEPTIONS=1 -g2 -s USE_GLFW=3 --shell-file ${CMAKE_SOURCE_DIR}/src/minshell.html --preload-file data -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy -lidbfs.js -s FORCE_FILESYSTEM=1 -sEXPORTED_RUNTIME_METHODS=ccall -sEXPORTED_FUNCTIONS=_success,_main -sASYNCIFY -sGL_ENABLE_GET_PROC_ADDRESS")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wno-variadic-macro-arguments-omitted)
endif()

if(WIN64CC)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".exe")
endif()

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /WX /W4 /wd4189 /wd4100)
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
  
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
  
  file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.h")
  source_group("Header Files" FILES ${PROJECT_HEADERS})
  target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_HEADERS})

else()
if(WIN64CC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib -lgdi32 -lwinmm -static-libgcc -static-libstdc++)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-missing-field-initializers)
    if(EMSCRIPTEN)
      target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
    else()
      target_link_libraries(${PROJECT_NAME} PRIVATE raylib stdc++fs)
    endif()
  endif()
endif()
