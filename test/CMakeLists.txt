add_executable(test_http_status test_http_status.cpp)
target_link_libraries(test_http_status PRIVATE uv_a server_template)
add_test(should_get_corresponding_status ../bin/test_http_status)

add_executable(test_http_method test_http_method.cpp)
target_link_libraries(test_http_method PRIVATE uv_a server_template)
add_test(should_get_corresponding_method ../bin/test_http_method)