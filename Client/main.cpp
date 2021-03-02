//
// Created by Mahmud Jego on 2/24/21.
//

#include "Client.hpp"

int main()
{
	Client client("127.0.0.1", 8000);
	client.start();
}
