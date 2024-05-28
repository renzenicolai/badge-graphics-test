cmake_minimum_required(VERSION 3.10)

project(linux_gfx)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC
    .
    pax-graphics/src
)

# Link to libm
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# Link to pthread
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

# Link to SDL2
find_package(SDL2 QUIET)
if (SDL2_FOUND)
    message("Found SDL2")
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
else()
    message( FATAL_ERROR "Can't find SDL2" )
endif()

# Link to PAX graphics
set(PAX_CMAKE_STANDALONE Linux)
add_subdirectory(pax-graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pax-graphics/libpax_graphics.a)

