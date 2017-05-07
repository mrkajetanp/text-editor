include_directories("../src/include")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/glib-2.0/include")
include_directories("/usr/local/include/glib-2.0")
include_directories("/usr/local/lib/glib-2.0/include")

add_executable(logic_test logic_test.c)

target_link_libraries(logic_test editor)
target_link_libraries(logic_test gap_buffer)

target_link_libraries(logic_test ncurses)
target_link_libraries(logic_test glib-2.0)
target_link_libraries(logic_test check)

add_test(logic-test logic_test)
