add_executable(test_http_status test_http_status.cpp)
target_link_libraries(test_http_status PRIVATE uv_a server_template)
add_test(should_get_corresponding_status ../bin/test_http_status)

add_executable(test_http_method test_http_method.cpp)
target_link_libraries(test_http_method PRIVATE uv_a server_template)
add_test(should_get_corresponding_method ../bin/test_http_method)

add_executable(test_http_response test_http_response.cpp)
target_link_libraries(test_http_response PRIVATE uv_a server_template)
add_test(should_get_corresponding_response ../bin/test_http_response)

add_executable(test_byte_array test_byte_array.cpp)
target_link_libraries(test_byte_array PRIVATE uv_a server_template)
add_test(should_get_byte_array ../bin/test_byte_array)

add_executable(test_uri test_uri.cpp)
target_link_libraries(test_uri PRIVATE uv_a server_template)
add_test(should_parse_uri ../bin/test_uri)

add_executable(test_http_parser test_http_parser.cpp)
target_link_libraries(test_http_parser PRIVATE uv_a server_template)
add_test(should_parse_http_message ../bin/test_http_parser)

add_executable(test_base64 test_base64.cpp)
target_link_libraries(test_base64 PRIVATE uv_a server_template)
add_test(should_encode_base64 ../bin/test_base64)

add_executable(test_byte_converter test_byte_converter.cpp)
target_link_libraries(test_byte_converter PRIVATE uv_a server_template)
add_test(should_convert_bytes ../bin/test_byte_converter)

add_executable(test_byte_converter_array test_byte_converter_array.cpp)
target_link_libraries(test_byte_converter_array PRIVATE uv_a server_template)
add_test(should_convert_array ../bin/test_byte_converter_array)

add_executable(test_sha_1 test_sha_1.cpp)
target_link_libraries(test_sha_1 PRIVATE uv_a server_template)
add_test(should_gen_sha_1_hash ../bin/test_sha_1)

add_executable(test_ws_frame_parser test_ws_frame_parser.cpp)
target_link_libraries(test_ws_frame_parser PRIVATE uv_a server_template)
add_test(should_parse_ws_frame ../bin/test_ws_frame_parser)