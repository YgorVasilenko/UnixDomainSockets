cmake_minimum_required(VERSION 3.16)

project(unix_domain_sockets VERSION 1.0)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_compile_definitions(
	server PRIVATE
	BUF_LEN=8192
	SERVER_ADDRESS="server.sock"
)

target_compile_definitions(
	client PRIVATE
	BUF_LEN=8192
	SERVER_ADDRESS="server.sock"
	CLIENT_ADDRESS="client.sock"
)
