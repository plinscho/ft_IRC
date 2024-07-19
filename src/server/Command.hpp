#ifndef COMAND_HPP
#define COMAND_HPP

#include "StringHandler.hpp"
#include "../messages/Messages.hpp"
#include <string>
#include <vector>

struct Command 
{
    public:
    StringHandler               strTool;
    Messages                    message;
	std::vector<std::string>    m_params;

};

#endif // COMMAND_HPP