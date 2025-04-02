cmake_minimum_required(VERSION 3.30.0)

# Add example executables
add_executable(simple_input simple_input.c)
add_executable(snake_game snake_game.c)
add_executable(text_editor text_editor.c)
add_executable(colors colors.c)

# Link against libconsole
target_link_libraries(simple_input PRIVATE libconsole)
target_link_libraries(snake_game PRIVATE libconsole)
target_link_libraries(text_editor PRIVATE libconsole)
target_link_libraries(colors PRIVATE libconsole)

# Set include directories
target_include_directories(simple_input PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(snake_game PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(text_editor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(colors PRIVATE ${CMAKE_SOURCE_DIR}/include)
