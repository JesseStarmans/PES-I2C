#include "SocketServer.h"

int main(void) {
	const char IP[16] = "145.52.127.169";
	SocketServer server(8080, IP);
	server.listenForClients();
}
