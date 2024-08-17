#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

static int handlePart(Client &user, Server &server, const std::string &channelName)
{
	std::string response;
	Channel *currentChnl = server.getChannelByName(channelName);

	if (!currentChnl) 
	{
		response = "Error. " + channelName + " does not exist.\r\n";
		server.message.sendMessage(user, response);
		return 0;
	}

	if (currentChnl->getChannelName() == channelName) 
	{
		std::map<int, Client*>::iterator it2 = currentChnl->_fdUsersMap.begin();
		while (it2 != currentChnl->_fdUsersMap.end()) 
		{
			int fd = it2->first;
			std::string nickname = it2->second->getNickname();
			// Respuesta para el servidor (channel)
			response = ":" + user.getPrefix() + " PART " + channelName + "\r\n";
			currentChnl->broadcastMessage(response);

			// Eliminar usuario del canal
			currentChnl->removeUser(fd);
			std::cout << "Parted user with fd: " << fd << std::endl;

			// Eliminar operador si es necesario
			if (std::find(currentChnl->nickOp.begin(), currentChnl->nickOp.end(), nickname) != currentChnl->nickOp.end()) {
				currentChnl->removeOpUser(nickname);
			}

			// Avanzar el iterador después de la eliminación
			it2++;
		}

		if (currentChnl->activeUsers == 0)
			server.deleteChannel(channelName);

		return 0;
	}

	return 1;
}

int Command::cmdPart(Client &user, Server &server, std::string channelNames)
{
	std::string response;

	std::vector<std::string> channelsPart = strTool.stringSplit(channelNames, ',');
	// Iterar sobre cada nombre de canal
	for (size_t i = 0; i < channelsPart.size(); ++i) {
		std::string channelName = channelsPart[i];
		// Manejar la salida del canal
		if (handlePart(user, server, channelName)) {
			std::string response = "Error. No such channel: " + channelName + "\r\n";
			message.sendMessage(user, response);
		}
	}
	return 0;
}