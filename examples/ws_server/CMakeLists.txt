file(GLOB WS_SERVER_FILES 
"*.cpp"
"*.h"
"endpoints/*.h"
"extensions/*.h"
)

add_executable(ws_server ${WS_SERVER_FILES})

target_link_libraries(ws_server PRIVATE uv_a server_template)