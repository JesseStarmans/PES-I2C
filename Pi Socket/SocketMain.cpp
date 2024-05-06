#include "SocketServer.h"

int main(void) {
	SocketServer server(8080);
	server.listenForClients();
}
