cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(linux_gfx)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC
    .
    pax-graphics/src
)

# Link to libm and pthread
target_link_libraries(${PROJECT_NAME} m)
target_link_libraries(${PROJECT_NAME} pthread)

# Link to SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Link to PAX graphics
set(PAX_CMAKE_STANDALONE Linux)
add_subdirectory(pax-graphics)
target_link_libraries(${PROJECT_NAME} pax_graphics)

