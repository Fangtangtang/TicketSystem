/*
 * used to deal with timestamp, cmd, parameters in each line
 * store the string form of the parameter
 * convert to expected form
 */
#ifndef TICKETSYSTEM_PARAMETER_HPP
#define TICKETSYSTEM_PARAMETER_HPP

#include <string>
#include <iostream>
#include "tokenScanner.hpp"

class Parameter {
    int timeStamp = 0;
    std::string para[25];

public:

    /*
     * read a new line and phase with tokenScanner
     * return cmd
     */
    std::string ReadLine();

    /*
     * get parameter of key
     * return false if not exist
     * string
     */
    bool GetParameter(const char &key, std::string &str) const;

    /*
     * get parameter of key
     * return false if not exist
     * int
     */
    bool GetParameter(const char &key, int &num) const;

    /*
     * get parameter of key
     * return false if not exist
     * char*
     */
    bool GetParameter(const char &key, char *str) const;

    /*
     * get parameter of key
     * return false if not exist
     * char
     */
    bool GetParameter(const char &key, char &ch) const;

    int GetTimestamp() const;
};

std::string Parameter::ReadLine() {
    for (auto &i: para) {
        i.clear();
    }
    std::string input;
    std::getline(std::cin, input);
    TokenScanner tokenScanner(input);
    tokenScanner.ReadTimestamp(timeStamp);
    tokenScanner.NextToken(input);//cmd
    char ch;
    while (tokenScanner.HasMoreTokens()) {
        tokenScanner.ReadKey(ch);
        tokenScanner.NextToken(para[ch - 'a']);
    }
    return input;
}

bool Parameter::GetParameter(const char &key, std::string &str) const {
    if (para[key - 'a'].empty()) return false;
    str = para[key - 'a'];
    return true;
}

bool Parameter::GetParameter(const char &key, int &num) const {
    if (para[key - 'a'].empty()) return false;
    num = 0;
    size_t index = key - 'a', i = 0, len = para[index].size();
    while (i < len) {
        num *= 10;
        num += (para[index][i] - '0');
        ++i;
    }
    return true;
}

bool Parameter::GetParameter(const char &key, char *str) const {
    if (para[key - 'a'].empty()) return false;
    strcpy(str, para[key - 'a'].c_str());
    return true;
}

bool Parameter::GetParameter(const char &key, char &ch) const {
    if (para[key - 'a'].empty()) return false;
    ch = para[key - 'a'][0];
    return true;
}

int Parameter::GetTimestamp() const {
    return timeStamp;
}


#endif //TICKETSYSTEM_PARAMETER_HPP
