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

    //下一个token为timestamp
    void ReadTimestamp(int &intNum);

    //下一个token为key
    void ReadKey(char &ch);

    //下一个合法token为string
    void NextToken(std::string &str);

private:
    std::string input;

    int length = 0;

    //每次截取token的始末位置
    int tokenStart = 1, tokenEnd = 1;

    //更新token头尾部所在位置
    void UpdatePos();
};

TokenScanner::TokenScanner(const std::string &str) {
    input = str;
    length = str.length();
    tokenStart = tokenEnd = 1;
}

bool TokenScanner::HasMoreTokens() const {
    if (tokenEnd >= length) return false;
    else return true;
}

void TokenScanner::ReadTimestamp(int &intNum) {
    intNum = 0;
    while (input[tokenEnd] != ']') {
        intNum *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            intNum += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
    }
    ++tokenEnd;
    UpdatePos();
}

void TokenScanner::ReadKey(char &ch) {
    ++tokenEnd;
    ch = input[tokenEnd];
    UpdatePos();
}

void TokenScanner::NextToken(std::string &str) {
//    std::cout<<"\n===============";
    while (input[tokenEnd] != ' '&&input[tokenEnd] != '\r') {
//        std::cout<<input[tokenEnd]<<' ';
        ++tokenEnd;
        if (tokenEnd == length) break;
    }
    str = input.substr(tokenStart, tokenEnd - tokenStart);
    UpdatePos();
}

//private
void TokenScanner::UpdatePos() {
    if (tokenEnd >= length - 1) {
        ++tokenEnd;
        return;
    }
    tokenStart = tokenEnd + 1;
    while (input[tokenStart] == ' '||input[tokenEnd] == '\r') {
        ++tokenStart;
        if (tokenStart == length) break;
    }
    tokenEnd = tokenStart;
}


#endif //TICKETSYSTEM_TOKENSCANNER_HPP
