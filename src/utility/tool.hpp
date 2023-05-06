/*
 * toolClasses
 */
#ifndef TICKETSYSTEM_TOOL_HPP
#define TICKETSYSTEM_TOOL_HPP

#include <cstring>

struct CompareUsername;

class Username {
    char username[20] = {'\0'};
    friend CompareUsername;
public:
    Username() = default;

    explicit Username(char *username_);

};

Username::Username(char *username_) {
    memset(username, 0, sizeof(username));
    strcpy(username, username_);
}

struct CompareUsername {
    bool operator()(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username) < 0;
    }

    int CompareStr(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username);
    }
};

class Time {

};

#endif //TICKETSYSTEM_TOOL_HPP
