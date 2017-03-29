include_directories("include")

add_executable(text-editor main.c input.c render.c)
add_subdirectory(lib)
add_subdirectory(include)

install(TARGETS text-editor
  RUNTIME DESTINATION bin)

target_link_libraries(text-editor gap_buffer)
target_link_libraries(text-editor check)
target_link_libraries(text-editor ncurses)
