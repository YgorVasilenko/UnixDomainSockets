#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

// sockets
int m_socket = -1;
int client_socket = -1;

// addresses
struct sockaddr_un m_addr;
struct sockaddr_un client_addr;
socklen_t client_addr_len;

// message buffer
char msg_buff[BUF_LEN];

int exitAndPrintError(const char * syscall_name, bool do_unlink = true) {
	perror(syscall_name);
	close(m_socket);
	if (do_unlink)
		unlink(SERVER_ADDRESS);
		
	return -1;
}

int main() {
	// clear message buffer
	memset(msg_buff, 0, BUF_LEN * sizeof(char));
	
	// get socket
	m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (m_socket == -1) {
		return exitAndPrintError("socket", false); 
	}
	
	// specify family name
	m_addr.sun_family = AF_UNIX;

	// make sure server address is free
	unlink(SERVER_ADDRESS);
	
	// bind socket to address
	strcpy(m_addr.sun_path, SERVER_ADDRESS);
	if (bind(m_socket, (const sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
		return exitAndPrintError("bind", false); 
	};
	
	// listen for incoming connections
	if (listen(m_socket, 50) == -1) {
               return exitAndPrintError("listen");
        }
        
        std::cout << "Waiting for connection to appear..." << std::endl;
        
        // with address name length specified "accept" will give us client_add.sun_path
        client_addr_len = sizeof(client_addr);
        
        // accept connection
	client_socket = accept(m_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_socket == -1) {
               return exitAndPrintError("accept");
        }
	std::cout << "Got connection!" << std::endl;
	std::cout << "Client address: " << client_addr.sun_path << std::endl;
	std::cout << "Enter 'r' to recieve message, 'q' to quit" << std::endl;
	
	// manually controlled "read" cycle	
	int nread = 0;
	char ctrl;
	while (true) {
		std::cin >> ctrl;
		if (ctrl == 'r') {
			nread = recv(client_socket, msg_buff, BUF_LEN * sizeof(char), 0);
			if (nread == -1) {
				perror("recv");
				continue;
			}
			if (nread == 0) {
				std::cout << "Client broke the connection!" << std::endl;
				break;	
			}
			std::cout << "Message recieved: " << msg_buff << std::endl;
			std::cout << "Message length: " << nread << std::endl; 
		}
		if (ctrl == 'q') {
			break;
		}
	}
	std::cout << "Closing socket "<< m_socket << std::endl;
	close(m_socket);
	unlink(SERVER_ADDRESS);
	return 0;
}
