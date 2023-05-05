#ifndef TICKETSYSTEM_TOKENSCANNER_HPP
#define TICKETSYSTEM_TOKENSCANNER_HPP

#include <string>
#include <cctype>

class TokenScanner {
public:
    TokenScanner() = default;

    explicit TokenScanner(const std::string &str);

    ~TokenScanner() = default;

    //除去前导空格
    void Initialize();

    //是否仍有输入
    bool HasMoreTokens() const;

    //下一个合法token为string
    void NextToken(std::string &str);

    //下一个合法token为char*
    void NextToken(char *token, int validSize, bool allowQuote);

    //特殊要求的char*
    void SpecialNextToken(char *token);

    //下一个合法token为int
    void NextToken(int &intNum, bool isPri);

    //下一个合法token为double
    void NextToken(double &doubleNum);

    //解析 -string=
    void TakeType(std::string &str);

    //解析”char*"
    void Quote(char *token);

    //给出剩余部分的string
    std::string ShowRest();

    std::string ShowOperation();

private:
    std::string input;
    std::string operation;
    //input 长度
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

void TokenScanner::Initialize() {
    while (input[tokenStart] == ' ') {
        ++tokenStart;
        if (tokenStart == length) break;
    }
    tokenEnd = tokenStart;
}

bool TokenScanner::HasMoreTokens() const {
    if (tokenEnd >= length) return false;
    else return true;
}

void TokenScanner::NextToken(std::string &str) {
    while (input[tokenEnd] != ' ') {
//        if (!std::isprint(input[tokenEnd]))error("Invalid");
        ++tokenEnd;
        if (tokenEnd == length) break;
    }
    str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + str + " ";
    UpdatePos();
}

void TokenScanner::NextToken(char *token, int validSize, bool allowQuote) {
    while (input[tokenEnd] != ' ') {
//        if (!std::isprint(input[tokenEnd]))error("Invalid");
//        if (!allowQuote) {
//            if (input[tokenEnd] == '"')error("Invalid");
//        }
        ++tokenEnd;
//        if (tokenEnd - tokenStart > validSize)error("Invalid");
        if (tokenEnd == length) break;
    }
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + str + " ";
    const char *tmp = str.c_str();
    strcpy(token, tmp);
    UpdatePos();
}

void TokenScanner::SpecialNextToken(char *token) {
    while (input[tokenEnd] != ' ') {
        if ((input[tokenEnd] - '0' < 10 && input[tokenEnd] - '0' >= 0) || input[tokenEnd] == '_' ||
            (input[tokenEnd] - 'a' >= 0 && input[tokenEnd] - 'a' < 26) ||
            (input[tokenEnd] - 'A' >= 0 && input[tokenEnd] - 'A' < 26)) {
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
//        else error("Invalid");
    }
//    if (tokenEnd - tokenStart > 30)error("Invalid");
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + str + " ";
    const char *tmp = str.c_str();
    strcpy(token, tmp);
    UpdatePos();
}

void TokenScanner::NextToken(int &intNum, bool isPri) {
    intNum = 0;
    long long num = 0;
//    if (input[tokenStart] == '0') {
//        if (tokenStart < length - 1 && input[tokenStart + 1] != ' ') error("Invalid");
//    }
    while (input[tokenEnd] != ' ') {
        num *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            num += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
//        else error("Invalid");
    }
//    if (isPri && tokenEnd - tokenStart != 1)error("Invalid");
//    if (num > 2147483647)error("Invalid");
    intNum = int(num);
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + str + " ";
    UpdatePos();
}

void TokenScanner::NextToken(double &doubleNum) {
    doubleNum = 0;
    bool double_flag = false;
//    if (input[tokenStart] == '0') {
//        if (tokenStart < length - 1 && (input[tokenStart + 1] != ' ' && input[tokenStart + 1] != '.')) error("Invalid");
//    }
//    if (input[tokenStart] == '.') error("Invalid");
    //整数部分
    while (input[tokenEnd] != ' ') {
        if (input[tokenEnd] == '.') {
            ++tokenEnd;
            double_flag = true;
            break;
        }
        doubleNum *= 10;
        if (input[tokenEnd] - '0' >= 0 && input[tokenEnd] - '0' < 10) {
            doubleNum += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
//        else error("Invalid");
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
//        else error("Invalid");
    }
//    if (count == 0 && double_flag) error("Invalid");
//    if (tokenEnd - tokenStart > 13) error("Invalid");
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + str + " ";
    UpdatePos();
}

void TokenScanner::TakeType(std::string &str) {
//    if (input[tokenStart] != '-') error("Invalid");
    ++tokenStart;
    tokenEnd = tokenStart;
    while (input[tokenEnd] != '=' && input[tokenEnd] != ' ') {
//        if (tokenEnd == length - 1) error("Invalid");
        ++tokenEnd;
    }
//    if (input[tokenEnd] != '=') error("Invalid");
//    if (input[tokenEnd + 1] == ' ') error("Invalid");
    str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + "-" + str + "=";
    tokenStart = tokenEnd + 1;
    tokenEnd = tokenStart;
}

void TokenScanner::Quote(char *token) {
//    if (input[tokenStart] != '"') error("Invalid");
    ++tokenStart;
    tokenEnd = tokenStart;
    while (input[tokenEnd] != '"' && input[tokenEnd] != ' ') {
//        if (!std::isprint(input[tokenEnd]))error("Invalid");
//        if(input[tokenEnd] == '"'){
//            if(tokenEnd!=length-1&&input[tokenEnd+1]!=' ') error("Invalid");
//        }
//        if (tokenEnd == length - 1) error("Invalid");
        ++tokenEnd;
    }
//    if (input[tokenEnd] != '"') error("Invalid");
//    if (tokenEnd - tokenStart > 60 || tokenEnd - tokenStart == 0)error("Invalid");
    std::string str = input.substr(tokenStart, tokenEnd - tokenStart);
    operation = operation + '"' + str + '"' + " ";
    const char *tmp = str.c_str();
    strcpy(token, tmp);
    UpdatePos();
}

std::string TokenScanner::ShowRest() {
    return input.substr(tokenStart, length - tokenStart);
}

std::string TokenScanner::ShowOperation() {
    return operation;
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
