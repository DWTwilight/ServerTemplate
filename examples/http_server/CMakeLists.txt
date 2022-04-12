add_executable(http_server main.cpp)

target_link_libraries(http_server PRIVATE uv_a server_template)