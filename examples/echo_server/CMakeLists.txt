file(GLOB ECHO_SERVER_FILES 
"*.cpp"
"*.h"
"echo_protocol/*.h"
)

add_executable(echo_server ${ECHO_SERVER_FILES})

target_link_libraries(echo_server PRIVATE uv_a server_template)