include_directories("../src/include")

add_executable(main_test main_test.c)

target_link_libraries(main_test gap_buffer)
target_link_libraries(main_test check)

add_test(main-test main_test)
