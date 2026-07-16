cmake_minimum_required(VERSION 3.15)
project(SFML_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML CONFIG REQUIRED COMPONENTS graphics window system audio)

file(GLOB_RECURSE LIB_SOURCES
        "external/*.cpp"
        "src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        SFML_STATIC
        WITHOUT_THREAD_SAFE_LOGGER
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC
            "-framework OpenGL"
            "-framework Foundation"
            "-framework AppKit"
            "-framework IOKit"
            "-framework Carbon"
            "-framework CoreGraphics"
            "-framework CoreVideo"
            "-framework CoreAudio"
            "-framework AudioToolbox"
    )
endif()

file(GLOB ENGINE_SOURCES
        "engine_data/src/*.cpp"
)

add_executable(${PROJECT_NAME}_engine
        ${ENGINE_SOURCES}
)

target_link_libraries(${PROJECT_NAME}_engine
        PRIVATE
        ${PROJECT_NAME}
)

target_include_directories(${PROJECT_NAME}_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/engine_data/src
)

target_compile_definitions(${PROJECT_NAME}_engine PRIVATE
        SFML_STATIC
        WITHOUT_THREAD_SAFE_LOGGER
)

set_target_properties(${PROJECT_NAME}_engine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/engine_data/build"
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

enable_testing()

add_test(NAME BuildLibraryTest
        COMMAND ${CMAKE_COMMAND} --build . --target ${PROJECT_NAME}
)

message(STATUS "SFML linked statically into ${PROJECT_NAME}")