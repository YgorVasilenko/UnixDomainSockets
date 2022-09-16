#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

struct sockaddr_un m_addr;
char msg_buff[BUF_LEN];
struct sockaddr_un server_addr;
int m_socket;

int exitAndPrintError(const char * syscall_name, bool do_unlink = true) {
	perror(syscall_name);
	close(m_socket);
	if (do_unlink)
		unlink(CLIENT_ADDRESS);
		
	return -1;
}

int sendData() {
	strcpy(msg_buff, "Hello, server");
	int send_count = send(m_socket, msg_buff, strlen(msg_buff), 0);
 	if (send_count == -1) {
 		return exitAndPrintError("send");
 	};
 	std::cout << "Sended " << send_count << " bytes to server..." << std::endl;
 	return send_count;
};

int main() {
	
	// create socket
	m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (m_socket == -1) {
		return exitAndPrintError("socket", false);
	}
	
	// make sure address is available	
	unlink(CLIENT_ADDRESS);
	
	// bind socket to address
	m_addr.sun_family = AF_UNIX;
	strcpy(m_addr.sun_path, CLIENT_ADDRESS);
	if (bind(m_socket, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
		return exitAndPrintError("bind", false);
	};
	
	// connect to server
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SERVER_ADDRESS);
	if (connect(m_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		return exitAndPrintError("connect");
	};
	
	std::cout << "Connected to server: " << SERVER_ADDRESS << std::endl;
	std::cout << "Enter 's' to send data, 'q' to quit" << std::endl;
	
	// manually controlled "send" cycle
	char ctrl;
	while (true) {
		std::cin >> ctrl;
		if (ctrl == 'q') {
			break;
		}
		if (ctrl == 's') {
			sendData();
		}
	}

	std::cout << "Closing socket "<< m_socket << std::endl;
	close(m_socket);
	unlink(CLIENT_ADDRESS);
	return 0;
}
