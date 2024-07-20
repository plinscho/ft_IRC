#ifndef STRINGHANDLER_HPP
#define STRINGHANDLER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

struct StringHandler
{
    std::string                     stringToHex(const std::string& str);
    std::vector<std::string>        stringSplit(std::string str, std::string delimiter);
    std::vector<std::string>        stringSplit(std::string str, char c);
    std::vector<std::string>        stringNoTrimSplit(std::string str, std::string delimiter);
	std::string						strJoin(std::vector<std::string> &);
    void                            printBuffer(std::string buffer) { std::cout << "ReceiveData CMD recived:\n" << buffer << "##server: end of buffer.##" << std::endl; }
};


#endif