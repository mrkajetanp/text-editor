include_directories("include")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/glib-2.0/include")
include_directories("/usr/local/include/glib-2.0")
include_directories("/usr/local/lib/glib-2.0/include")

add_library(editor screen.c input.c render.c files.c)

target_link_libraries(editor gap_buffer)

add_executable(text-editor main.c)
add_subdirectory(lib)
add_subdirectory(include)

install(TARGETS text-editor
  RUNTIME DESTINATION bin)

target_link_libraries(text-editor editor)
target_link_libraries(text-editor gap_buffer)

target_link_libraries(text-editor ncurses)
target_link_libraries(text-editor glib-2.0)
target_link_libraries(text-editor check)

if(APPLE)
  target_link_libraries(text-editor argp)
endif()
