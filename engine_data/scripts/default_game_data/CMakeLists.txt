cmake_minimum_required(VERSION 3.15)
project(Default LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML CONFIG REQUIRED COMPONENTS graphics window system audio)

set(SFML_PROJECT_LIB "lib/libSFML_project.a")

add_library(SFML_project STATIC IMPORTED)

set_target_properties(SFML_project PROPERTIES
        IMPORTED_LOCATION ${SFML_PROJECT_LIB}
)

file(GLOB_RECURSE GAME_SOURCES
        "src/*.cpp"
)

add_executable(Default ${GAME_SOURCES})

target_link_libraries(Default PRIVATE
        SFML_project
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

target_compile_definitions(Default PRIVATE
        SFML_STATIC
        WITHOUT_THREAD_SAFE_LOGGER
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE external)

message(STATUS "Default sources: ${GAME_SOURCES}")