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
	std::string						strTrim(std::string s1, std::string delimiter);
    void                            printBuffer(std::string buffer) { std::cout << "ReceiveData CMD recived:\n" << buffer << "##server: end of buffer.##" << std::endl; }
};


#endif