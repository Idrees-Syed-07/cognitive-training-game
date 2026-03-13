cmake_minimum_required(VERSION 3.20)
project(cognitive-training-game C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use UCRT (Universal C Runtime)
if(MSVC)
    # MSVC uses UCRT by default with /MD
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
elseif(MINGW)
    # Ensure MinGW UCRT toolchain flags
    add_compile_definitions(_UCRT)
endif()

# Find GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_executable(${PROJECT_NAME}
    src/main.c
    src/maze.c
    src/gui.c
    src/player.c
    src/audio.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GTK4_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME} PRIVATE ${GTK4_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK4_LIBRARIES} winmm)
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK4_CFLAGS_OTHER})
