#ifndef TICKETSYSTEM_TOKENSCANNER_HPP
#define TICKETSYSTEM_TOKENSCANNER_HPP

#include <string>
#include <cstring>
#include <cctype>

class TokenScanner {
public:
    TokenScanner() = default;

    explicit TokenScanner(const std::string &str);

    ~TokenScanner() = default;

    //是否仍有输入
    bool HasMoreTokens() const;

    //下一个合法token为string
    void NextToken(std::string &str);

    //下一个合法token为char*
    void NextToken(char *token);

    //下一个合法token为int
    void NextToken(int &intNum);

    //下一个合法token为short
    void NextToken(short &shortNum);

    //下一个合法token为double
    void NextToken(double &doubleNum);

    //解析 -string=
    void TakeType(std::string &str);

    //解析"char*"
    void Quote(char *token);

    //给出剩余部分的string
    std::string ShowRest();


private:
    std::string input;

    int length = 0;

    //每次截取token的始末位置
    int tokenStart = 0, tokenEnd = 0;

    //更新token头尾部所在位置
    void UpdatePos();
};

TokenScanner::TokenScanner(const std::string &str) {
    input = str;
    length = str.length();
    tokenStart = tokenEnd = 0;
}

bool TokenScanner::HasMoreTokens() const {
    if (tokenEnd >= length) return false;
    else return true;
}

void TokenScanner::NextToken(std::string &str) {
    while (input[tokenEnd] != ' ') {
        ++tokenEnd;
        if (tokenEnd == length) break;
    }
    str = input.substr(tokenStart, tokenEnd - tokenStart);
    UpdatePos();
}

void TokenScanner::NextToken(char *token) {
    while (input[tokenEnd] != ' ') {
        ++tokenEnd;
        if (tokenEnd == length) break;
    }
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    strcpy(token, str.c_str());
    UpdatePos();
}


void TokenScanner::NextToken(int &intNum) {
    intNum = 0;
    long long num = 0;
    while (input[tokenEnd] != ' ') {
        num *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            num += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
    }
    intNum = int(num);
    UpdatePos();
}

void TokenScanner::NextToken(short &shortNum) {
    shortNum = 0;
    int num = 0;
    while (input[tokenEnd] != ' ') {
        num *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            num += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
    }
    shortNum = short(num);
    UpdatePos();
}

void TokenScanner::NextToken(double &doubleNum) {
    doubleNum = 0;
    //整数部分
    while (input[tokenEnd] != ' ') {
        if (input[tokenEnd] == '.') {
            ++tokenEnd;
            break;
        }
        doubleNum *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            doubleNum += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
    }
    double mul = 1;
    int count = 0;
    while (tokenEnd < length && input[tokenEnd] != ' ') {
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            mul *= 0.1;
            doubleNum += (input[tokenEnd] - '0') * mul;
            ++count;
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
    }
    UpdatePos();
}

void TokenScanner::TakeType(std::string &str) {
    ++tokenStart;
    tokenEnd = tokenStart;
    while (input[tokenEnd] != '=' && input[tokenEnd] != ' ') {
        ++tokenEnd;
    }
    tokenStart = tokenEnd + 1;
    tokenEnd = tokenStart;
}

void TokenScanner::Quote(char *token) {
    ++tokenStart;
    tokenEnd = tokenStart;
    while (input[tokenEnd] != '"' && input[tokenEnd] != ' ') {
        ++tokenEnd;
    }
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    strcpy(token, str.c_str());
    UpdatePos();
}

std::string TokenScanner::ShowRest() {
    return input.substr(tokenStart, length - tokenStart);
}

//private
void TokenScanner::UpdatePos() {
    if (tokenEnd >= length - 1) {
        ++tokenEnd;
        return;
    }
    tokenStart = tokenEnd + 1;
    while (input[tokenStart] == ' ') {
        ++tokenStart;
        if (tokenStart == length) break;
    }
    tokenEnd = tokenStart;
}

#endif //TICKETSYSTEM_TOKENSCANNER_HPP
