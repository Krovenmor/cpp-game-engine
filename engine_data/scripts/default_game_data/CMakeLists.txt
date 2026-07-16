cmake_minimum_required(VERSION 3.15)
project(Default LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML CONFIG REQUIRED COMPONENTS graphics window system audio)

set(SFML_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../SFML_proj")

if(NOT EXISTS "${SFML_PROJECT_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "Библиотека не найдена по пути: ${SFML_PROJECT_ROOT}")
endif()

set(SFML_PROJECT_LIB "${SFML_PROJECT_ROOT}/build/libSFML_project.a")
set(SFML_PROJECT_INCLUDE "${SFML_PROJECT_ROOT}/include")

add_library(SFML_project STATIC IMPORTED)

set_target_properties(SFML_project PROPERTIES
        IMPORTED_LOCATION ${SFML_PROJECT_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${SFML_PROJECT_INCLUDE}
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

message(STATUS "Default sources: ${GAME_SOURCES}")