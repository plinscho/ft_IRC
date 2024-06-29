#include "Server.hpp"

class Server;

int Server::updatePoll(void)
{

	int ret;
	// call poll() one time and update the _vectorPoll vector.
	ret = poll(_vectorPoll.data(), _vectorPoll.size(), POLL_TIMEOUT);
	if (ret < 0)
	{
		if (errno == EINTR)
			return (0);
		else
			return (quickError("Error.\nPoll() function failed.", EXIT_FAILURE));
	}
	else if (ret == 0)
		return (quickError("Server timed out.\n", EXIT_FAILURE));
	return (0);
}

