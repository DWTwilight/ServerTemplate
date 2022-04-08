set(ST_LIB_NAME server_template)

file(GLOB ST_LIB_FILES 
"server_template/*.h"
"server_template/*.cpp"
"server_template/base/*.h"
"server_template/tcp/*.h"
"server_template/util/*.h"
"server_template/http/*.h"
"server_template/ws/*.h"
)

add_library(${ST_LIB_NAME} STATIC ${ST_LIB_FILES})

set_target_properties(${ST_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${ST_LIB_NAME} PRIVATE uv_a)

target_include_directories(${ST_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})