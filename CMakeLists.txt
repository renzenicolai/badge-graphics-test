cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(linux_gfx)

set(TARGET ${PROJECT_NAME})

aux_source_directory(. SRC_LIST)
aux_source_directory(fonts FONT_SRC_LIST)
add_executable(${TARGET} ${SRC_LIST} ${FONT_SRC_LIST})

target_include_directories(${TARGET} PUBLIC
    .
    pax-graphics/src
)

# Link to libm and pthread
target_link_libraries(${TARGET} m)
target_link_libraries(${TARGET} pthread)

# Link to SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${TARGET} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${SDL2_LIBRARIES})

# Link to PAX graphics
set(PAX_CMAKE_STANDALONE Linux)
add_subdirectory(pax-graphics)
target_link_libraries(${TARGET} pax_graphics)

# Link to PAX codecs
add_subdirectory(pax-codecs)
target_link_libraries(${TARGET} pax_codecs)
