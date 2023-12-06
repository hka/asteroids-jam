cmake_minimum_required(VERSION 3.0)
project(asteroids-jam C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Adding Raylib
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(imGui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG docking)
FetchContent_GetProperties(imGui)
if(NOT imGui_POPULATED)
  FetchContent_Populate(imGui)
endif()

FetchContent_Declare(rlImGui GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git GIT_TAG main)
FetchContent_GetProperties(rlImGui)
if(NOT rlImGui_POPULATED)
  FetchContent_Populate(rlImGui)
endif()

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
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") 

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS "${imgui_SOURCE_DIR}/*.cpp")

# Declaring our executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCES}
  ${rlimgui_SOURCE_DIR}/rlImGui.cpp
  ${IMGUI_SOURCES}
)
target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_INCLUDE}
  ${visit_struct_SOURCE_DIR}/include/
  ${json_SOURCE_DIR}/single_include/
  ${rlimgui_SOURCE_DIR}/
  ${imgui_SOURCE_DIR}/
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)

if (EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME}
          PROPERTIES SUFFIX ".html"
          LINK_FLAGS " --bind -s WASM=1 -s MIN_WEBGL_VERSION=1 -s ABORT_ON_WASM_EXCEPTIONS=1 -g2 -s USE_GLFW=3 --shell-file ${CMAKE_SOURCE_DIR}/src/minshell.html --preload-file data -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy -lidbfs.js -s FORCE_FILESYSTEM=1 -sEXPORTED_RUNTIME_METHODS=ccall -sEXPORTED_FUNCTIONS=_success -sASYNCIFY")
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
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-missing-field-initializers)
  if (EMSCRIPTEN)
     target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib stdc++fs)
  endif()
endif()

