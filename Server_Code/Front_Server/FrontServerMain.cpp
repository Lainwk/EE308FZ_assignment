#include <iostream>
#include "FrontGatewayServer.h"
using namespace std;

int main() {
	FrontGatewayServer* a = new FrontGatewayServer(10001);
	a->epoll_event_start();

	return 0;
}
